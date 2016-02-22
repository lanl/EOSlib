#include "VonMisesConstGV.h"
#include "ConstGV.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *VonMisesConstGVlib_vers = VonMisesConstGV_vers;
const char *VonMisesConstGVlib_date = VonMisesConstGV_date;

extern "C" { // DataBase functions
  char *EOS_VonMisesConstGV_Init()
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
  char *EOS_VonMisesConstGV_Info()
  {
      std::string msg(VonMisesConstGVlib_vers);
      msg = msg + " : " + VonMisesConstGVlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_VonMisesConstGV_Constructor()
  {
      EOS *eos = new VonMisesConstGV;
      return static_cast<void*>(eos);
  }
}

VonMisesConstGV::VonMisesConstGV() : ElasticPlastic1D("VonMisesConstGV")
{
    z_ref = new PlasticStrain1;
    hydro   = NULL;
    shear   = NULL;
    Y0 = nu = 0.0;
    A1 = A2 = 0.0;
    n1 = n2 = 1.0;
    eps_y   = 0.0;
    G0 = gv = 0.0;
    eps_y_compression = eps_y_expansion = 0.0;
}

VonMisesConstGV::VonMisesConstGV(const VonMisesConstGV &VM)
                    : ElasticPlastic1D(VM)
{
    Y0 = VM.Y0;
    nu = VM.nu;
    A1 = VM.A1;
    A2 = VM.A2;
    n1 = VM.n1;
    n2 = VM.n2;
    eps_y = VM.eps_y;
    G0    = gv = 0.0;
    eps_y_compression = VM.eps_y_compression;
    eps_y_expansion   = VM.eps_y_expansion;
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

VonMisesConstGV::~VonMisesConstGV()
{
    // Null
}

ExtEOS *VonMisesConstGV::Copy()
{
    VonMisesConstGV *eos = new VonMisesConstGV(*this);
    return eos;

}


void VonMisesConstGV::PrintParams(ostream &out)
{
    ElasticPlastic1D::PrintParams(out);
    out << "\tY0 = " << Y0  << "\n";
    out << "\tnu = " << nu << "\n";
    if( A1 > 0 )
        out << "\tA1 = " << A1 << "; \tn1 = " << n1 << "\n";
    if( A2 > 0)
        out << "\tA2 = " << A2 << "; \tn2 = " << n2 << "\n";
    if( eps_y > 0 )
    {
        out << "\t#\teps_y             = " << eps_y              << "\n"
            << "\t#\teps_y_compression = "  << eps_y_compression << "\n"
            << "\t#\teps_y_expansion   =  " << eps_y_expansion   << "\n";
    }
    out << "\t#\ttau   = " << nu/G0 << "\n";
}

int VonMisesConstGV::ConvertParams(Convert &convert)
{
    double s_p, s_t, s_e;
    if( !finite(s_p=convert.factor("P"))
         || !finite(s_t=convert.factor("t"))
         || !finite(s_e=convert.factor("e")) )
    {
        EOSerror->Log("VonMisesConstGV::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    Y0  *= s_p;
    nu  *= s_p*s_t;
    G0  *= s_p;
    gv  *= s_e;
    return ElasticPlastic1D::ConvertParams(convert);
}

int VonMisesConstGV::PreInit(Calc &calc)
{
    Y0 = nu = NaN;
    A1 = A2 = 0.0;
    n1 = n2 = 1;
    eps_y   = 0;
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
    G0 = gv = 0.0;
    eps_y_compression = eps_y_expansion = 0.0;    
    return ElasticPlastic1D::PreInit(calc);
}

int VonMisesConstGV::PostInit(Calc &calc, DataBase *db)
{
    if( ElasticPlastic1D::PostInit(calc,db) )
        return 1;
    hydro   = elastic->hydro;
    shear   = elastic->shear;    
    if( isnan(Y0) || Y0 < 0 || isnan(nu) || nu < 0)
    {
        EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "Y0 or nu not defined or not positive\n");
        return 1;      
    }
    ConstGV *GVshear = dynamic_cast<ConstGV*>(shear);
    if( GVshear == NULL )
    {
        EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "shear not type ConstGV\n");
        return 1;      
    }
    gv = GVshear->GV;
    G0 = gv/V_ref;
    if( G0 <= 0.0 )
    {
        EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "G0 <= 0\n");
        return 1;      
    }
    double tau;
    calc.Fetch("tau",tau);
    if( tau > 0 )
        nu = G0*tau;
    if( nu <= 0.0 )
    {
        EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "nu <= 0\n");
        return 1;      
    }
    if( Y0 > 0 )
    {
        eps_y = 0.5*Y0/G0;
        eps_y_expansion = inverse(Y0,0,2*eps_y);
        if( OneDFunction::Status() )
        {
            EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "inverse failed for eps_y_expansion\n");
            return 1;
        }
        eps_y_compression = inverse(Y0,0,-2*eps_y);
        if(  OneDFunction::Status() )
        {
            EOSerror->Log("VonMisesConstGV::PostInit",__FILE__,__LINE__,this,
                      "inverse failed for eps_y_compression\n");
            return 1;
        }        
    }
    return 0;
}
