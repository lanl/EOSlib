#include "Polymer.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *Polymerlib_vers = Polymer_vers;
const char *Polymerlib_date = Polymer_date;

extern "C" { // DataBase functions
  char *EOS_Polymer_Init()
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
  char *EOS_Polymer_Info()
  {
      std::string msg(Polymerlib_vers);
      msg = msg + " : " + Polymerlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_Polymer_Constructor()
  {
      EOS *eos = new Polymer;
      return static_cast<void*>(eos);
  }
}

Polymer::Polymer() : ElasticPlastic1D("Polymer")
{
    z_ref  = new PlasticStrain1;
    hydro  = NULL;
    Pshear = NULL;
    tau0 = eps0 = NaN;
    n = 0.;
    depsdeta0 = 0.;
    depsdeta1 = NaN;    // natural spline, second derivative = 0
    Vmin      = 0.;
       n_data = 0;
    epsy_data = NULL;
}

Polymer::Polymer(const Polymer &VM) : ElasticPlastic1D(VM)
{
    tau0 = VM.tau0;
    eps0 = VM.eps0;
    n    = VM.n;
    if( elastic == NULL )
    {
        hydro = NULL;
        Pshear = NULL;
        epsy_data = NULL;
    }
    else
    {
        hydro = VM.elastic->hydro;
        Pshear = dynamic_cast<PolyG*>(VM.elastic->shear);
    }  
    depsdeta0 = VM.depsdeta0;
    depsdeta1 = VM.depsdeta1;
    Vmin      = VM.Vmin;
    n_data    = VM.n_data;
    epsy_data = new double[n_data];
    int i;
    for( i=0; i<n_data; i++ )
         epsy_data[i] = VM.epsy_data[i];
    (void)Reinit();
}

Polymer::~Polymer()
{
    delete [] epsy_data;    
}

int Polymer::Reinit()
{
    double eta0 = 0.;
    double eta1 = 1.-Vmin/V_ref;
    double fp_0 = depsdeta0;
    double fp_1 = depsdeta1;
    int status = eps_y.ReInitCopy(n_data, eta0,eta1, epsy_data, fp_0,fp_1);
    if( status )
    {
	    EOSerror->Log("Polymer::Reinit",__FILE__,__LINE__,NULL,
                      "epsy.ReInit failed\n");
        return 1;
    }
    return 0;
}

ExtEOS *Polymer::Copy()
{
    Polymer *eos = new Polymer(*this);
    return eos;

}


void Polymer::PrintParams(ostream &out)
{
    ElasticPlastic1D::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    out << "\ttau0 = " << tau0 << "\n"
        << "\teps0 = " << eps0 << "\n"
        << "\t   n = " << n    << "\n"
        << "\tVmin = " << Vmin << "\n";
    if( !std::isnan(depsdeta0) )
	    out << "\t" << setw(10) << "depsdeta0 = " << depsdeta0 << "\n";
    if( !std::isnan(depsdeta1) )
	    out << "\t" << setw(10) << "depsdeta1 = " << depsdeta1 << "\n";
    if( n_data > 0 )
    {
        int i;
        out << "\t#n_data = " << n_data << "\n";
        for( i=0; i<n_data; i+=5 )
        {
            int j;
            int jmax = min(i+5,n_data);
            out << "\tepsy(" << i+1 << ") = " << setw(10) << epsy_data[i];
            for( j=i+1; j<jmax; j++ )
            {
                out << ", " << setw(10) << epsy_data[j];
            }
            out << "\n";
        }
    }
	out.setf(old, ios::adjustfield);  
}

int Polymer::ConvertParams(Convert &convert)
{
    double s_V, s_t;
    if( !finite(s_V=convert.factor("V"))
         || !finite(s_t=convert.factor("time")) )
    {
        EOSerror->Log("Polymer::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    Vmin  *= s_V;
    tau0  *= s_t;
    return ElasticPlastic1D::ConvertParams(convert);
}

int Polymer::PreInit(Calc &calc)
{
    tau0 = eps0 = NaN;
    n = 0.;
    depsdeta0 = 0.;
    depsdeta1 = NaN;    // natural spline, second derivative = 0
    Vmin      = 0.;
       n_data = 0;
    delete [] epsy_data;
    epsy_data = NULL;
    calc.Variable("tau0", &tau0);
    calc.Variable("eps0", &eps0);
    calc.Variable("n", &n);    
    calc.Variable("depsdeta0", &depsdeta0);
    calc.Variable("depsdeta1", &depsdeta1);
    calc.Variable("Vmin", &Vmin);    
    calc.DynamicArray("epsy",128);
    hydro   = NULL;
    Pshear   = NULL;
    return ElasticPlastic1D::PreInit(calc);
}

int Polymer::PostInit(Calc &calc, DataBase *db)
{
    if( ElasticPlastic1D::PostInit(calc,db) )
        return 1;
    hydro   = elastic->hydro;
    Pshear = dynamic_cast<PolyG*>(elastic->shear);
    if( Pshear == NULL )
    {
        EOSerror->Log("Polymer::PostInit",__FILE__,__LINE__,this,
                      "shear not type PolyG\n");
        return 1;      
    }
    if( std::isnan(tau0) || tau0 < 0. || std::isnan(eps0) || eps0 < 0.)
    {
        EOSerror->Log("Polymer::PostInit", __FILE__, __LINE__, this,
                      "tau0 or eps0 not defined or not positive\n");
        return 1;      
    }
    n_data = calc.FetchDynamicArray("epsy",epsy_data);
    if( n_data < 2 )
    {
        EOSerror->Log("Polymer::PostInit", __FILE__, __LINE__, this,
                      "n_data < 2\n");
        return 1;      
    }    
    return Reinit();
}
