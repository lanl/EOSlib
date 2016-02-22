#include "DavisReactants.h"
#include <iostream>
#include <iomanip>
using namespace std;

DavisReactants::DavisReactants() : EOS("DavisReactants")
{
    V0 = NaN;
    e0 = 0.;
    P0 = 1.e-4;
    S0 = 0.;
    T0 = 293.;
    A = B = C = G0 = Z = Cv = alpha = NaN;
    ps = 0.0;
}

DavisReactants::~DavisReactants()
{
	// Null
}

void DavisReactants::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t" << setw(8) << "V0 = " << V0 << "\n"
	    << "\t" << setw(8) << "e0 = " << e0 << "\n"
	    << "\t" << setw(8) << "P0 = " << P0 << "\n"
	    << "\t" << setw(8) << "T0 = " << T0 << "\n"
	    << "\t" << setw(8) << "S0 = " << S0 << "\n"
	    << "\t" << setw(8) << " A = " <<  A << "\n"
	    << "\t" << setw(8) << " B = " <<  B << "\n"
	    << "\t" << setw(8) << " C = " <<  C << "\n"
	    << "\t" << setw(8) << "G0 = " << G0 << "\n"
	    << "\t" << setw(8) << " Z = " <<  Z << "\n"
	    << "\t" << setw(8) << "Cv = " << Cv << "\n"
	    << "\t" << setw(8) << "alpha = " << alpha << "\n";
	out.setf(old, ios::adjustfield);      
}

int DavisReactants::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_u, s_T;
	if(  !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	        || !finite(s_u = convert.factor("u"))
	           || !finite(s_T = convert.factor("T")) )
	{
	    EOSerror->Log("DavisReactants::ConvertParams",__FILE__,__LINE__,NULL,
                      "failed\n");
	    return 1;
	}	
	V0 *= s_V;
	e0 *= s_e;
	P0 *= s_e/s_V;
	T0 *= s_T;
	S0 *= s_e/s_T;
	A  *= s_u;
	Cv *= s_e/s_T;
	ps *= s_e/s_V;
	return 0;
}

#define FUNC "DavisReactants::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int DavisReactants::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0 = NaN;
    e0 = 0.;
    P0 = 1.e-4;
    S0 = 0.;
    T0 = 293.;
    A = B = C = G0 = Z = Cv = alpha = NaN;
    ps = 0.0;
    calcvar(V0);
    calcvar(e0);
    calcvar(P0);
    calcvar(S0);
    calcvar(T0);
    calcvar(A);
    calcvar(B);
    calcvar(C);
    calcvar(G0);
    calcvar(Z);
    calcvar(Cv);
    calcvar(alpha);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);   
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( isnan(V0) || V0<=0.0 )
    {
        EOSerror->Log(FUNC,"V0 not set or invalid");
        return 1;
    }
    if( isnan(A) || A<=0. || isnan(B) || B<=0. || isnan(C) || C<=0. )
    {
        EOSerror->Log(FUNC,"A, B or C not set or invalid");
        return 1;
    }
    if( isnan(G0) || G0 <= 0. || isnan(Z) )
    {
        EOSerror->Log(FUNC,"G0 or Z not set or invalid");
        return 1;
    }
    if( isnan(Cv) || Cv<=0 || isnan(alpha) || alpha < 0. )
    {
        EOSerror->Log(FUNC,"Cv or alpha not set or invalid");
        return 1;
    }    
    ps = 0.25*A*A/B/V0;
    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(e_ref) )
        e_ref = e0;
    return 0;
}
