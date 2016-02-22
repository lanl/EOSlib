#include "ConstGV.h"

#include <iostream>
#include <iomanip>
using namespace std;
#include <string>


const char *ConstGVlib_vers  = ConstGV_vers;
const char *ConstGVlib_date  = ConstGV_date;

extern "C" { // dynamic link for database
  char *Shear1D_ConstGV_Init()
  {
      if( Shear1Dlib_mismatch(Shear1D_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Shear1Dlib_vers
                  + "), derived(" + Shear1D_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *Shear1D_ConstGV_Info()
  {
      std::string msg(ConstGVlib_vers);
      msg = msg + " : " + ConstGVlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *Shear1D_ConstGV_Constructor()
  {
    Shear1D *shear = new ConstGV;
	return static_cast<void *>(shear);
  }
}

ConstGV::ConstGV() : Shear1D("ConstGV")
{
    GV = 0;
}

ConstGV::~ConstGV()
{
    // Null
}

void ConstGV::PrintParams(ostream &out)
{
    out << "\tGV = " << GV << "\n";    
}

int ConstGV::ConvertParams(Convert &convert)
{
    double s_e;
    if( !finite(s_e=convert.factor("e")) )
        return 1;
	GV *= s_e;
    return 0;   
}

#define FUNC "ConstGV::InitParams",__FILE__,__LINE__,this
int ConstGV::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    GV = 0;
    calc.Variable("GV",&GV);
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    return 0;
}
