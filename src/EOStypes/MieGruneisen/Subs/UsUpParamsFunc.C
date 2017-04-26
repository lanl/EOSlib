#include "UsUp.h"

double UsUpParams::Pref(double V)
{
    double v = V/V0;
    return P0 + K0*(1-v)/sqr(1-s*(1-v));
}

double UsUpParams::dPref(double V)
{
    double v = V/V0;
    return -(K0/V0)*(1+s*(1-v))/(1-s*(1-v))/sqr(1-s*(1-v));
}

double UsUpParams::d2Pref(double V)
{
    double v = V/V0;
    return 2*s*K0*(2+s*(1-v))/sqr(V0*sqr(1-s*(1-v)));
}

double UsUpParams::eref(double V)
{
    return e0+ 0.5*(P0+Pref(V))*(V0-V);
}

double UsUpParams::deref(double V)
{
    return 0.5*(V0-V)*dPref(V)-0.5*(P0+Pref(V));
}

double UsUpParams::d2eref(double V)
{
    return 0.5*(V0-V)*d2Pref(V)-dPref(V);
}

double UsUpParams::Tref(double V)
{
    double Tv[1];
    int stat;
    if( (stat=ODE::Integrate(V,Tv,NULL)) )
    {
	    /*************
        EOSerror->Log("UsUpParams::Tref",__FILE__,__LINE__,this,
                      "ODE.Integrate failed with status: %s\n",
	    	          ODE::ErrorStatus(stat) );
        *************/
        return EOS::NaN;
    }
    return exp(-intGoV(V))*(T0 + 0.5*Tv[0]/Cv);
}

int UsUpParams::F(double *yp, const double *y, double V)
{
    if( V/V0 <= 1 - 1/s )
    {
        //cerr << "V/V0=" << V/V0 << ",  1-1/s=" << 1-1/s << "\n";
        return 1;
    }
    yp[0] = (dPref(V)*(V0-V) + Pref(V)-P0)*exp(intGoV(V));
    return 0;
}

double UsUpParams::GoV(double V)
{
    return (G0-G1)/V + G1/V0;
}

double UsUpParams::dGoV(double V)
{
    return -(G0-G1)/sqr(V);
}

double UsUpParams::d2GoV(double V)
{
    return 2*(G0-G1)/(V*V*V);
}

double UsUpParams::intGoV(double V)
{
    return (G0-G1)*log(V/V0) + (G1/V0)*(V-V0);
}

double UsUpParams::c2ref(double V)
{
    double eta = 1 - V/V0;
    double us = c0/(1-s*eta);
    double up = eta*us;
    return sqr((V/V0)*us)*(1 +(2-GoV(V)*V0*eta)*(us/c0-1));
}
