#include <EOSbase.h>
#include "HayesParams.h"
#include <iostream>
#include <iomanip>
using namespace std;

HayesParams::HayesParams()
{
    V0 = e0 = P0 = T0 = EOSbase::NaN;
    K0 = N = Gamma0 = Cv = EOSbase::NaN;
}

int HayesParams::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_T;
	if(  !finite(s_V = convert.factor("V"))
	      || !finite(s_e = convert.factor("e"))
	          || !finite(s_T = convert.factor("T")) )
	{
	    return 1;
	}
	
	V0 *= s_V;
	e0 *= s_e;
	P0 *= s_e/s_V;
	T0 *= s_T;
	
	K0 *= s_e/s_V;
	Cv *= s_e/s_T;
		
	return 0;
}

#define calcvar(var) calc.Variable(#var,&var)
void HayesParams::InitParams(Calc &calc)
{
    V0 = e0 = P0 = T0 = EOSbase::NaN;
    K0 = N = Gamma0 = Cv = EOSbase::NaN;
	calcvar(V0);
	calcvar(e0);
	calcvar(P0);
	calcvar(T0);
	calcvar(K0);
	calcvar(N);
	calcvar(Gamma0);
	calcvar(Cv);
}

void HayesParams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 << "\n"
	    << "\t"   << setw(10) << "P0 = " << P0
	    << "; \t" << setw(10) << "T0 = " << T0 << "\n"
	    << "\t"   << setw(10) << "K0 = " << K0
	    << "; \t" << setw(10) << " N = " << N  << "\n"
	    << "\t"   << setw(10) << "Gamma0 = " << Gamma0
	    << "; \t" << setw(10) << "Cv = " << Cv << "\n";
    
	double c0 = sqrt(V0*K0+Gamma0*Gamma0*Cv*T0);
	double R  = Gamma0*Cv*T0/(K0*V0);
	double s  = 0.25*(N + 1+Gamma0*Gamma0*R)/(1 + Gamma0*R);
	out << "#\t"  << setw(10) << "c0 = " << c0
	    << "; \t" << setw(10) << " s = " << s  << "\n";

	out.setf(old, ios::adjustfield);	
}

int HayesParams::ParamsOK() const
{
    return finite(V0) && finite(e0) && finite(P0) && finite(T0)
           && finite(K0) && finite(N)  && finite(Gamma0) && finite(Cv);
}
