#include "ElasticPlasticTypes.h"
#include <OneDFunction.h>

#include <string>
#include <cstring>


const char *VonMiseslib_vers = VonMises_vers;
const char *VonMiseslib_date = VonMises_date;

extern "C" { // DataBase functions
  char *EOS_VonMises_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());        
      }
      if( strcmp(ElasticPlastic_vers, ElasticPlastic_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ElasticPlastic_lib_vers + "), derived("
                  + ElasticPlastic_vers + ")\n";
        return strdup(msg.c_str());        
      }
      if( strcmp(VonMises_vers, VonMiseslib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + VonMiseslib_vers + "), derived("
                  + VonMises_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_VonMises_Info()
  {
      std::string msg(VonMiseslib_vers);
      msg = msg + " : " + VonMiseslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_VonMises_Constructor()
  {
      EOS *eos = new VonMises;
      return static_cast<void*>(eos);
  }
}

VonMises::VonMises() : ElasticPlastic("VonMises")
{
     Y = NaN;
    nu = NaN;
}

VonMises::~VonMises()
{
    // Null
}

VonMises::VonMises(const VonMises &VM) : ElasticPlastic(VM)
{
     Y = VM.Y;
    nu = VM.nu;
}

ExtEOS *VonMises::Copy()
{
    VonMises *eos = new VonMises(*this);
    return eos;
}

int VonMises::PreInit(Calc &calc)
{
    Y = nu = NaN;
    calc.Variable("Y",  &Y);
    calc.Variable("nu", &nu);
    //
    A1 = A2 = 0.0;
    n1 = 1;
    tau = 0;
    eps_y = 0;
    calc.Variable("A1",  &A1);
    calc.Variable("A2",  &A2);
    calc.Variable("tau", &tau);
    calc.Variable("n",   &n1);    
    return ElasticPlastic::PreInit(calc);
}

int VonMises::PostInit(Calc &calc, DataBase *db)
{
    if( ElasticPlastic::PostInit(calc,db) )
        return 1;
    if( std::isnan(Y) || Y < 0 || std::isnan(nu) || nu < 0)
    {
        EOSerror->Log("VonMises::PostInit", __FILE__, __LINE__, this,
                      "Y or nu not defined or not positive\n");
        return 1;      
    }
    if( Y > 0 )
    {
        double Te = elastic->T_ref;
        double Ve = elastic->V_ref;
        double eps_e = elastic->eps_el(Ve);
        double G = elastic->dshear(Ve,Te, eps_e);
        eps_y = Y/G;
        if( tau != 0 )
            nu = Y*tau;
    }
    return 0;
}

void VonMises::PrintParams(ostream &out)
{
    ElasticPlastic::PrintParams(out);
    out << "\tY  = " << Y  << "\n";
    out << "\tnu = " << nu << "\n";
    //
    //
    if( A1 )
        out << "\t A1 = " << A1 << "; \tn = " << n1
            << " \t#\teps_y = " << eps_y << "\n";
    if( A2 )
        out << "\t A2 = " << A2 << "\n";
    if( tau )
        out << "\ttau = " << tau << "\n";
}

int VonMises::ConvertParams(Convert &convert)
{
    double s_p, s_t;
    if( !finite(s_p=convert.factor("pressure"))
         || !finite(s_t=convert.factor("time")) )
    {
        EOSerror->Log("VonMises::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    Y  *= s_p;
    nu *= s_p*s_t;
    tau *= s_t;
    return ElasticPlastic::ConvertParams(convert);
}

double VonMises::yield_func(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : (V/V_ref)*std::abs(elastic->shear(V,Tve, z_el[0]));
}

double VonMises::yield_surf(double V, double e, const double *z)
{
    return Y;
}

int VonMises::Rate(double V, double e, const double *z, double *zdot)
{
    zdot[0] = 0;
    double Tve = T(V,e, z);
    if( std::isnan(Tve) )
        return -1;

    double shear = elastic->shear(V,Tve, z_el[0]);   
    double Yf = (V/V_ref)*std::abs(shear);
    if( Yf <= Y )
        zdot[0] = 0;
    else
    {
        zdot[0] = (3./4.)*(Yf-Y)/nu * (V/V_ref)*(shear/Yf);
        if( Y > 0 )
            zdot[0] *= (1+A1*pow(std::abs(z[0])/eps_y,n1)+A2*(Yf/Y-1));
    }
    return 1;
}

int VonMises::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    double Tve = T(V,e, z);
    if( std::isnan(Tve) )
        return -1;  // error

    double shear = elastic->shear(V,Tve, z_el[0]);   
    double Yf = (V/V_ref)*std::abs(shear);
    if( Yf <= Y )
        return 0;   // no constraint
    double G = elastic->dshear(V,Tve, z_el[0]); // 2*(isothermal shear modulus)
    dt = (4./3.)*nu *(V_ref/V)/G;
    if( Y > 0 )
       dt /= (1+A1*pow(std::abs(z[0])/eps_y,n1)+A2*(Yf/Y-1));
    return 2;   // assuming dln(G)/deps << 1
}

int VonMises::Integrate(double V, double e, double *z, double dt)
{
    double z0 = z[0];
    int status = Step(V,e,z,dt);
    if( status < 1 || dt <= 0 )
        return status == -1;
    int count = 25;    
    while( count-- )
    {
        status = Step(V,e,z,dt);
        if( status < 0 )
            return 1;
        else if( status == 0 )
            break;
        else if( dt <= 0 )
            return 0;
    }
    if( status == 0 )
        z[0] = z0;
    return Equilibrate(V,e,z);  
}

int VonMises::Step(double V, double e, double *z, double &t)
{
    double Tve = T(V,e, z);
    if( std::isnan(Tve) )
        return -1;  // error

    double shear = elastic->shear(V,Tve, z_el[0]);   
    double Yf = (V/V_ref)*std::abs(shear);
    if( Yf < Y )
        return 0;
    double G = elastic->dshear(V,Tve, z_el[0]);
    double deps = (V_ref/V)*(Yf-Y)/G;       // for Y = Yf
    double tau = (4./3.)*nu *(V_ref/V)/G;
    if( Y > 0 )
        tau /= (1+A1*pow(std::abs(z[0])/eps_y,n1)+A2*(Yf/Y-1));
    if( shear < 0 )
        deps = -deps;
    double dt = min(0.5*tau,t);
    t -= dt;
    z[0] += deps*(1-exp(-dt/tau));
    return 1;
}

class VonMises::ZonYield : public OneDFunction
{
public:
    double V;
    double e;
    VonMises &VM;
public:
    ZonYield(VonMises &vm, double V0, double e0) : V(V0), e(e0),VM(vm) { }
    double f(double eps);
};

double VonMises::ZonYield::f(double eps)
{
    double z[1];
    z[0] = eps;
    return VM.yield_func(V,e,z);   
}

int VonMises::Equilibrate(double V, double e, double *z)
{
    double Tve = T(V,e, z);
    if( std::isnan(Tve) )
        return -1;  // error

    double shear = elastic->shear(V,Tve, z_el[0]);
    double Yz0 = (V/V_ref)*std::abs(shear);
    if( Yz0 < Y + 10*P_vac )
        return 0;

    double sign_s = (shear>0) ? 1. : -1.;
    double z1[1];
    double z0 = z1[0] = z[0];
    int count = 10;
    while( count-- )
    {
        double G = elastic->dshear(V,Tve, z_el[0]);
        z1[0] += sign_s * (V_ref/V)*(Yz0-Y)/G;
        Tve = T(V,e,z1);
        double Yz1 = (V/V_ref)*std::abs(elastic->shear(V,Tve, z_el[0]));
        if( std::abs(Yz1 - Y) < 1e-6*Y + 10*P_vac )
        {
            z[0] = z1[0];
            return 0;
        }
        if( (Yz0 < Y && Y < Yz1) || (Yz1 < Y && Y < Yz0) )
        {
            ZonYield yield(*this,V,e);
            double eps = yield.inverse(Y,z0,Yz0,z1[0],Yz1);
            if( yield.Status() )
            {
                EOSerror->Log("VonMises::Equilibrate", __FILE__, __LINE__, this,
                       "failed with status, %s\n", yield.ErrorStatus() );
                return 1;
            }
            z[0] = eps;
            return 0;
        }   
        z0 = z1[0];
        Yz0 = Yz1;
    }
    ZonYield yield(*this,V,e);
    double eps = yield.inverse(Y,0,log(V/V_ref));
    if( yield.Status() )
    {
        EOSerror->Log("VonMises::Equilibrate", __FILE__, __LINE__, this,
                      "failed, V=%lf, e=%lf, z[0]=%lf\n", V,e,z[0] );
        return 1;
    }
    z[0] = eps;
    return 0;
}
