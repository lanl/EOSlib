#include "VonMises1D.h"
#include "ConstGV.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *VonMises1Dlib_vers = VonMises1D_vers;
const char *VonMises1Dlib_date = VonMises1D_date;

extern "C" { // DataBase functions
  char *EOS_VonMises1D_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(ElasticPlastic1D_vers, ElasticPlastic1D_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ElasticPlastic1D_lib_vers + "), derived("
                  + ElasticPlastic1D_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *EOS_VonMises1D_Info()
  {
      std::string msg(VonMises1Dlib_vers);
      msg = msg + " : " + VonMises1Dlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_VonMises1D_Constructor()
  {
      EOS *eos = new VonMises1D;
      return static_cast<void*>(eos);
  }
}

VonMises1D::VonMises1D() : ElasticPlastic1D("VonMises1D")
{
    z_ref   = new PlasticStrain1;
    hydro   = NULL;
    shear   = NULL;
    Y0 = nu = 0.0;
    A1 = A2 = 0.0;
    n1 = n2 = 1.0;
    eps_y   = 0.0;
}

VonMises1D::VonMises1D(const VonMises1D &VM) : ElasticPlastic1D(VM)
{
    Y0 = VM.Y0;
    nu = VM.nu;
    A1 = VM.A1;
    A2 = VM.A2;
    n1 = VM.n1;
    n2 = VM.n2;
    eps_y = VM.eps_y;
    if( elastic == NULL )
    {
        hydro = NULL;
        shear = NULL;
    }
    else
    {
        hydro = VM.elastic->hydro;
        shear = VM.elastic->shear;
    }
}

VonMises1D::~VonMises1D()
{
    // Null
}

ExtEOS *VonMises1D::Copy()
{
    VonMises1D *eos = new VonMises1D(*this);
    return eos;

}


void VonMises1D::PrintParams(ostream &out)
{
    ElasticPlastic1D::PrintParams(out);
    out << "\tY0 = " << Y0  << "\n";
    out << "\tnu = " << nu << "\n";
    if( A1 > 0 )
        out << "\tA1 = " << A1 << "; \tn1 = " << n1 << "\n";
    if( A2 > 0)
        out << "\tA2 = " << A2 << "; \tn2 = " << n2 << "\n";
    if( eps_y > 0 )
        out << "\t#\teps_y = " << eps_y << "\n";
    double Vref   = hydro->V_ref;
    double eref   = hydro->e_ref;
    double Tref   = hydro->T(Vref,eref);
    double eps_el = elastic->eps_el_ref;
    double G0 = (3./4.)*shear->dPdev_deps(Vref,Tref, eps_el);
    double tau = nu/G0;
    out << "\t#\ttau   = " << tau << "\n";
}

int VonMises1D::ConvertParams(Convert &convert)
{
    double s_p, s_t;
    if( !finite(s_p=convert.factor("pressure"))
         || !finite(s_t=convert.factor("time")) )
    {
        EOSerror->Log("VonMises1D::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    Y0  *= s_p;
    nu  *= s_p*s_t;
    return ElasticPlastic1D::ConvertParams(convert);
}

int VonMises1D::PreInit(Calc &calc)
{
    Y0 = nu = NaN;
    A1 = A2 = 0.0;
    n1 = n2 = 1.0;
    eps_y   = 0.0;
    calc.Variable("Y0", &Y0);
    calc.Variable("nu", &nu);
    calc.Variable("A1", &A1);
    calc.Variable("n1", &n1);    
    calc.Variable("A2", &A2);
    calc.Variable("n2", &n2);    
    double tau = 0.0;
    calc.Variable("tau",tau);
    hydro   = NULL;
    shear   = NULL;
    return ElasticPlastic1D::PreInit(calc);
}

int VonMises1D::PostInit(Calc &calc, DataBase *db)
{
    if( ElasticPlastic1D::PostInit(calc,db) )
        return 1;
    hydro   = elastic->hydro;
    shear   = elastic->shear;    
    if( std::isnan(Y0) || Y0 < 0 || isnan(nu) || nu < 0)
    {
        EOSerror->Log("VonMises::PostInit", __FILE__, __LINE__, this,
                      "Y0 or nu not defined or not positive\n");
        return 1;      
    }
    double Vref   = hydro->V_ref;
    double eref   = hydro->e_ref;
    double Tref   = hydro->T(Vref,eref);
    double eps_el = elastic->eps_el_ref;
    double G0 = (3./4.)*shear->dPdev_deps(Vref,eref, eps_el);
    double tau;
    calc.Fetch("tau",tau);
    if( tau > 0 )
        nu = G0*tau;
    if( nu <= 0.0 )
    {
        EOSerror->Log("VonMises1D::PostInit",__FILE__,__LINE__,this,
                      "nu <= 0\n");
        return 1;      
    }
    if( Y0 > 0 )
        eps_y = 0.5*Y0/G0;
    return 0;
}
