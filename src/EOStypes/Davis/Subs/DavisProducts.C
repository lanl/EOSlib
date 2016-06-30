#include "DavisProducts.h"
#include <iostream>
#include <iomanip>
using namespace std;


DavisProducts::DavisProducts() : EOS("DavisProducts")
{
    V0 = e0 =S0 = NaN;
    a = n = Vc = pc = b = k = Cv = NaN;
    ec = Tc = T0 = 0.0;
}

DavisProducts::~DavisProducts()
{
	// Null
}

void DavisProducts::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t" << setw(8) << "V0 = " << V0 << "\n"
	    << "\t" << setw(8) << "e0 = " << e0 << "\n"
	    << "\t" << setw(8) << "S0 = " << S0 << "\n"
	    << "\t" << setw(8) << " a = " <<  a << "\n"
	    << "\t" << setw(8) << " n = " <<  n << "\n"
	    << "\t" << setw(8) << "Vc = " << Vc << "\n"
	    << "\t" << setw(8) << "pc = " << pc << "\n"
	    << "\t" << setw(8) << "Tc = " << Tc << "\n"
	    << "\t" << setw(8) << " b = " <<  b << "\n"
	    << "\t" << setw(8) << " k = " <<  k << "\n"
	    << "\t" << setw(8) << "Cv = " << Cv << "\n";
	out.setf(old, ios::adjustfield);      
}

int DavisProducts::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_T;
	if(  !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	        || !finite(s_T = convert.factor("T")) )
	{
	    EOSerror->Log("DavisProducts::ConvertParams",__FILE__,__LINE__,NULL,
                      "failed\n");
	    return 1;
	}	
	V0 *= s_V;
	e0 *= s_e;
	S0 *= s_e/s_T;
	Vc *= s_V;
	pc *= s_e/s_V;
	Cv *= s_e/s_T;
	ec *= s_e;
	Tc *= s_T;
	T0 *= s_T;
	return 0;
}

#define FUNC "DavisProducts::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int DavisProducts::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0 = e0 =S0 = NaN;
    a = n = Vc = pc = b = k = Cv = NaN;
    ec = Tc = T0 = 0.0;
    calcvar(V0);
    calcvar(e0);
    calcvar(S0);
    calcvar(a);
    calcvar(n);
    calcvar(Vc);
    calcvar(pc);
    calcvar(b);
    calcvar(k);
    calcvar(Cv);
    calcvar(Tc);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);   
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( std::isnan(V0) || V0<=0.0 || isnan(e0) )
    {
        EOSerror->Log(FUNC,"V0 or e0 not set or invalid");
        return 1;
    }
    if( std::isnan(a) || a<=0 || isnan(n) || n<=0 )
    {
        EOSerror->Log(FUNC,"a or n not set or invalid");
        return 1;
    }
    if( std::isnan(Vc) || Vc<=0 || isnan(pc) || pc<=0 )
    {
        EOSerror->Log(FUNC,"Vc or pc not set or invalid");
        return 1;
    }
    if( std::isnan(b) || b>1 || isnan(k) || k<1 )
    {
        EOSerror->Log(FUNC,"b or k not set or invalid");
        return 1;
    }    
    if( std::isnan(Cv) || Cv<=0 )
    {
        EOSerror->Log(FUNC,"Cv not set or invalid");
        return 1;
    }    
    ec = pc*Vc/(k-1.+a);
    if( Tc <= 0.0 )
        Tc = pow(2,-a*b/n)/(k-1.+a) * pc*Vc/Cv;
    T0 = Tref(V0);
    if( std::isnan(S0) )
    {
        S0 = 0.0;
        double SV0 = S(V0,0.);       
        S0 = (e0-Cv*(T0-300.))/300. - SV0;
        // assumes Sreactants = 0 at V=V0 and e=0
        // and enthalpy difference(P=1 bar, T=300 K) = e0
    }
    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = 0;
    return 0;
}
