#include "ElasticTypes.h"

#include <string>
#include <cstring>

const char *Shear1lib_vers  = Shear1_vers;
const char *Shear1lib_date  = Shear1_date;

extern "C" { // DataBase functions
  char *EOS_VT_Shear1_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());        
      }
      if( strcmp(Elastic_VT_vers, Elastic_VTlib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Elastic_VTlib_vers + "), derived(" + Elastic_VT_vers + ")\n";
        return strdup(msg.c_str());        
      }
      if( strcmp(Shear1_vers, Shear1lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Shear1lib_vers + "), derived("
                  + Shear1_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_VT_Shear1_Info()
  {
      std::string msg(Shear1lib_vers);
      msg = msg + " : " + Shear1lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_VT_Shear1_Constructor()
  {
      EOS_VT *eos = new Shear1;
      return static_cast<void*>(eos);
  }
}


Shear1::Shear1() : Elastic_VT("Shear1")
{
    G = NaN;
}

Shear1::~Shear1()
{
    // Null
}

Shear1::Shear1(const Shear1 &s) : Elastic_VT(s)
{
    G = s.G;
}

ExtEOS_VT *Shear1::Duplicate(const double *znew)
{
    Shear1 *eos = new Shear1(*this);
    int i;
    for( i=0; i < ElasticStrain::n; i++ )
        eos->ElasticStrain::z[i] = znew[i];
    return eos;  
}

int Shear1::PreInit(Calc &calc)
{
    G = NaN;
    calc.Variable("G", &G);
      return 0;
}

int Shear1::PostInit()
{
    if( isnan(G) || G < 0 )
    {
        EOSerror->Log("Shear1::PostInit", __FILE__, __LINE__, this,
                      "G not defined or not positive\n");
        return 1;      
    }
    return 0;
}

void Shear1::PrintParams(ostream &out)
{
    Elastic_VT::PrintParams(out);
    out << "\tG  = " << G << "\n";
}

int Shear1::ConvertParams(Convert &convert)
{
    double s_p;
    if( !finite(s_p = convert.factor("pressure")) )
    {
        EOSerror->Log("Shear1::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    G *= s_p;
    return Elastic_VT::ConvertParams(convert);
}
//
//
double Shear1::Fshear(double V, double T, double eps_el)
{
    return eshear(V,T, eps_el);
}

double Shear1::Sshear(double V, double T, double eps_el)
{
    return 0;
}

double Shear1::eshear(double V, double T, double eps_el)
{
    double exp_el = exp((2./3.)*eps_el);
    return 0.5*V_ref*G*(exp_el*exp_el + 2/exp_el - 3);
}

double Shear1::Pshear(double V, double T, double eps_el)
{
    return 0;
}

double Shear1::shear(double V, double T, double eps_el)
{
    double exp_el = exp((2./3.)*eps_el);
    return G*(V_ref/V)*(exp_el*exp_el - 1/exp_el);
}

double Shear1::dshear(double V, double T, double eps_el)
{
    double exp_el  = exp((2./3.)*eps_el);
    return (2./3.)*G*(V_ref/V)*(2*exp_el*exp_el +1./exp_el);
}
    
double Shear1::stress(double V, double T, double eps_el)
{
    double exp_el = exp((2./3.)*eps_el);
    return (2./3.)*G*(V_ref/V)*(exp_el*exp_el - 1/exp_el);
}
    
double Shear1::stress_V(double V, double T, double eps_el)
{ // assume uniaxial flow, d(eps_el) = dV/V, i.e. d(eps_pl) = 0
    double exp_el = exp((2./3.)*eps_el);
    return (2./3.)*(G/V)*(V_ref/V)*( (1./3.)*exp_el*exp_el+(5./3.)/exp_el );
}

double Shear1::stress_T(double V, double T, double eps_el)
{
    return 0;
}

double Shear1:: CVshear(double V, double T, double eps_el)
{
    return 0;
}
