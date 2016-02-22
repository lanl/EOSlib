#include "PolyG.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *PolyGlib_vers  = PolyG_vers;
const char *PolyGlib_date  = PolyG_date;

extern "C" { // dynamic link for database
  char *Shear1D_PolyG_Init()
  {
      if( Shear1Dlib_mismatch(Shear1D_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Shear1Dlib_vers + "), derived(" + Shear1D_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *Shear1D_PolyG_Info()
  {
      std::string msg(PolyGlib_vers);
      msg = msg + " : " + PolyGlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *Shear1D_PolyG_Constructor()
  {
    Shear1D *shear = new PolyG;
	return static_cast<void *>(shear);
  }
}

PolyG::PolyG() : Shear1D("PolyG")
{
    V0 = 0;
}

PolyG::~PolyG()
{
    // Null
}

void PolyG::PrintParams(ostream &out)
{
    out << "\tV0 = " << V0 << "\n";
    G.print(out<<"\tG =") << "\n";
}

int PolyG::ConvertParams(Convert &convert)
{
    double s_V;
    double s_P;
    if( !finite(s_V=convert.factor("V"))
        || !finite(s_P=convert.factor("P")) )
        return 1;
	V0 *= s_V;
    int N = G.dim();
    int i;
    for( i=0; i<N; i++ )
      G[i] *= s_P;
    return 0;   
}

#define FUNC "PolyG::InitParams",__FILE__,__LINE__,this
int PolyG::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0 = 0;
    calc.Variable("V0",&V0);
    calc.DynamicArray("G",8);
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    double *g = NULL;
    int n = calc.FetchDynamicArray("G",g);
    G.ReInit(g,n);
    delete [] g;
    return 0;
}
