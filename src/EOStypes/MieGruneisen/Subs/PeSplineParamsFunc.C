#include "PeSpline.h"
#include "LocalMath.h"

double PeSplineParams::Pref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return P0+P.Evaluate(eta);   
    else if( V < Vmax )
        return P0 + K0*eta*(1+FD0*eta);
    else
        return Pmin;
}

double PeSplineParams::dPref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return -P.Evaluate1(eta)/V0;   
    else if( V < Vmax )
        return -K0/V0*(1+2*FD0*eta);
    else
        return 0.0;
}

double PeSplineParams::d2Pref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return P.Evaluate2(eta)/(V0*V0);   
    else if( V <= Vmax )
        return 2*FD0*K0/(V0*V0);
    else
        return 0.0;
}

double PeSplineParams::eref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return e0 + e.Evaluate(eta);
    else if( V <= Vmax )
        return e0 + P0*V0*eta + K0*V0*eta*eta*(0.5+FD0*eta/3);
    else
        return EOS::NaN;
}

double PeSplineParams::deref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return -e.Evaluate1(eta)/V0;   
    else if( V <= Vmax )
        return -(P0+K0*eta*(1+FD0*eta)); // deref/dV = -Pref(V)
    else
        return EOS::NaN;
}

double PeSplineParams::d2eref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
        return e.Evaluate2(eta)/(V0*V0);   
    else if( V <= Vmax )
        return K0/V0*(1+2*FD0*eta);       // d2eref = -dPref
    else
        return EOS::NaN;
}

double PeSplineParams::Tref(double V)
{
    if( V < V0 )
    {
        double Tv[1];
        int status;
        if( (status=ODE::Integrate(V,Tv,NULL)) )
        {
    	    /*************/
            EOSbase::ErrorDefault->Log("PeSplineParams::Tref",__FILE__,__LINE__,NULL,
                          "ODE.Integrate failed with status: %s\n",
    	    	          ODE::ErrorStatus(status) );
            /*************/
            return EOS::NaN;
        }
        return exp(-intGoV(V))*(T0 + Tv[0]/Cv);
    }
    else if( V <= Vmax )
        return T0*exp(-intGoV(V));
    else
        return EOS::NaN;
}

int PeSplineParams::F(double *yp, const double *y, double V)
{
    yp[0] = (Pref(V)+deref(V))*exp(intGoV(V));
    return 0;
}
/**************
double PeSplineParams::MaxNorm(const double *y0, const double *y1,
						       const double*, const double*)
{
// ToDo: upgrade ODE::MaxNorm for relative and absolute error
	double Norm = 0;
    double abserr = epsilon;
    double relerr = epsilon;
	
	for(int i=Dimension(); i--; y0++, y1++)
    {
        double e1 = abs(*y0 - *y1)/((abs(*y0)+abs(*y1) + relerr)*relerr);
        double e2 = abs(*y0 - *y1)/abserr;
		Norm = max(Norm, min(e1,e2) );
    }		
	return Norm;
}
*************/
double PeSplineParams::GoV(double V)
{
    //Gamma = G0 + G1*(V/V0-1) + G2*(V/V0-1)^2
    return (G0-G1+G2)/V + (G1-G2*(2.-V/V0))/V0;
}

double PeSplineParams::dGoV(double V)
{
    return -(G0-G1+G2)/sqr(V) + G2/sqr(V0);
}

double PeSplineParams::d2GoV(double V)
{
    return 2*(G0-G1+G2)/(V*V*V);
}

double PeSplineParams::intGoV(double V)
{
    double eta = V/V0 - 1.;
    return (G0-G1+G2)*log(V/V0) + (G1-G2)*eta + 0.5*G2*eta*eta;
}

double PeSplineParams::rhoc2ref(double V)
{
    double eta = 1-V/V0;
    if( eta > 0 )
        return -dPref(V) + GoV(V)*(Pref(V)+deref(V));
    else if( V <= Vmax )
        return K0/V0*(1+2*FD0*eta); // -dPref/dV
    else
        return EOS::NaN;
}

double PeSplineParams::drhoc2ref(double V)
{
    double eta = 1-V/V0;
    if( eta > 0 )
        return -d2Pref(V) +  GoV(V)*(dPref(V)+d2eref(V))
                          + dGoV(V)*(Pref(V)+deref(V));
    else if( V <= Vmax )
        return -2*FD0*K0/(V0*V0); // -d2Pref
    else
        return EOS::NaN;
}
