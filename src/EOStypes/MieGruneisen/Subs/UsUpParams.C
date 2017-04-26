#include "UsUp.h"
#include <iostream>
#include <iomanip>
using namespace std;

UsUpParams::UsUpParams() : ODE(1, 512)
{
    V0 = P0 = e0 = T0 = S0 = EOS::NaN;
    c0 = s= K0 = G0 = G1 = Cv = EOS::NaN;
    Vmax = Pmin = EOS::NaN;
}

UsUpParams::~UsUpParams()
{
    // Null
}

int UsUpParams::init(EOSbase::Error *EOSerror)
{
	K0 = c0*c0/V0;
    double V1 = (1+1/s)*V0;      // dPref(V1) = 0, c2ref(V1) < 0
    Vmax = V0;
    for( int i=0; i<20; i++ ) // bisection to find c2ref(Vmax)=0
    {
        double V = 0.5*(V1+Vmax);
        if( c2ref(V)>=0 )
            Vmax = V;
        else
            V1 = V;
    }
    Pmin = Pref(Vmax);
	double Tv[1];
	Tv[0] = 0.0;
    double dV = -0.01*V0/s;	
	int status = ODE::Initialize(V0, Tv, dV);
    if( status && EOSerror )
	    EOSerror->Log("UsUpParams::init",__FILE__,__LINE__,NULL,
                      "ODE.Initialize failed with status: %s\n",
	    	          ODE::ErrorStatus(status) );
    return status != 0;
}

int UsUpParams::copy(const UsUpParams &params)
{
    V0 = params.V0;
    P0 = params.P0;
    e0 = params.e0;
    T0 = params.T0;
    S0 = params.S0;
    c0 = params.c0;
    s  = params.s;
    G0 = params.G0;
    G1 = params.G1;
    Cv = params.Cv;
	return init(NULL);
}

#define calcvar(var) calc.Variable(#var,&var)
void UsUpParams::InitParams(Calc &calc)
{
    V0 = EOS::NaN;
    e0 = 0.;
    T0 = 300.;
    S0 = 0.;
    P0 = 1e-4;
    c0 = s = K0 = G0 = G1 = Cv = EOS::NaN;
    Vmax = Pmin = EOS::NaN;

    calcvar(V0);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
    calcvar(e0);
    calcvar(T0);
    calcvar(S0);
    calcvar(P0);
    calcvar(c0);
    calcvar(s);
    calcvar(G0);
    calcvar(G1);
    calcvar(Cv);    
}

void UsUpParams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 << "\n"
	    << "\t"   << setw(10) << "P0 = " << P0
	    << "; \t" << setw(10) << "T0 = " << T0
	    << "; \t" << setw(10) << "S0 = " << S0 << "\n"
	    << "\t"   << setw(10) << "c0 = " << c0
	    << "; \t" << setw(10) << "s  = " << s  << "\n"
	    << "\t"   << setw(10) << "Vmax = " << Vmax
	    << "; \t" << setw(10) << "Pmin = " << Pmin  << "\n"
	    << "\t"   << setw(10) << "G0 = " << G0
	    << "; \t" << setw(10) << "G1 = " << G1  << "\n"
	    << "\t" << setw(10) << "Cv = " << Cv << "\n";
	out.setf(old, ios::adjustfield);  
}

int UsUpParams::ConvertParams(Convert &convert, EOSbase::Error *EOSerror)
{
	double s_V, s_e, s_u, s_T;
	if(  !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	        || !finite(s_u = convert.factor("u"))
	           || !finite(s_T = convert.factor("T")) )
	{
	    EOSerror->Log("UsUpParams::ConvertParams",__FILE__,__LINE__,NULL,
                      "failed\n");
	    return 1;
	}
	
	V0 *= s_V;
	e0 *= s_e;
	P0 *= s_e/s_V;
	T0 *= s_T;
	S0 *= s_e/s_T;
	
    c0 *= s_u;
	Cv *= s_e/s_T;

	return init(EOSerror);
}

#define FUNC "UsUpParams::ParamsOK",__FILE__,__LINE__,NULL
int UsUpParams::ParamsOK(EOSbase::Error *EOSerror)
{
  if( std::isnan(V0) || std::isnan(e0) || std::isnan(P0) || std::isnan(T0) || std::isnan(S0) )
    {
        EOSerror->Log(FUNC,"variable not set: "
                           "V0=%lf, e0=%lf, P0=%lf, T0=%lf, S0=%lf\n",
                           V0, e0, P0, T0, S0);
        return 1;
    }
  if( std::isnan(c0) || std::isnan(s) || std::isnan(G0) || std::isnan(G1) || std::isnan(Cv) )
    {
        EOSerror->Log(FUNC,"variable not set: "
                           "c0=%lf, s=%lf, G0=%lf, G1=%lf, Cv=%lf\n",
                           c0, s, G0, G1, Cv);
        return 1;
    }
    if( V0 <= 0 || P0 < 0 || T0 < 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: V0=%lf, P0=%lf, T0=%lf\n",
                           V0, P0, T0);
        return 1;
    }
    if( c0 <= 0 || s<=1 )
    {
        EOSerror->Log(FUNC,"bad parameters: c0=%lf, s=%lf\n", c0, s);
        return 1;
    }
    if( G0 <= 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: G0=%lf, G1=%lf\n", G0, G1);
        return 1;
    }
    if( Cv <= 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: Cv=%lf\n", Cv);
        return 1;
    }
    return init(EOSerror);
}
