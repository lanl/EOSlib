#include "UsUpSpline.h"

double UsUpSplineParams::Pref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
    {
        double us = 1./ Us_inv.Evaluate(eta);   
        return P0 + eta*us*us/V0;
    }
    else if( V < Vmax )
        return P0 + K0*eta*(1+FD0*eta);
    else
        return Pmin;
}

double UsUpSplineParams::dPref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
    {
        double uinv[3];
        if( Us_inv.Evaluate(eta, uinv) )
            return EOS::NaN;
        double us = 1./uinv[0];
        return -sqr(us/V0)*(1.-2*eta*us*uinv[1]);
    }
    else if( V < Vmax )
        return -K0/V0*(1+2*FD0*eta);
    else
        return 0.0;
}

double UsUpSplineParams::d2Pref(double V)
{
    double eta = 1. - V/V0;
    if( eta > 0 )
    {
       double uinv[3];
       if( Us_inv.Evaluate(eta, uinv) )
           return EOS::NaN;
       double us = 1./uinv[0];
       double fpof = uinv[1]/uinv[0];
       return (-2/V0)*sqr(us/V0)*(fpof*(2.-3*eta*fpof)+eta*us*uinv[2]);
    }
    else if( V <= Vmax )
        return 2*FD0*K0/(V0*V0);
    else
        return 0.0;
}

double UsUpSplineParams::eref(double V)
{
    if( V < V0 )
        return e0+ 0.5*(P0+Pref(V))*(V0-V);
    else if( V <= Vmax )
    {
        double eta = 1-V/V0;
        return e0 + P0*V0*eta + K0*V0*eta*eta*(0.5+FD0*eta/3);
    }
    else
        return EOS::NaN;
}

double UsUpSplineParams::deref(double V)
{
    if( V < V0 )
        return 0.5*(V0-V)*dPref(V)-0.5*(P0+Pref(V));
    else if( V <= Vmax )
    {
        double eta = 1 - V/V0;
        return -(P0+K0*eta*(1+FD0*eta)); // deref/dV = -Pref(V)
    }
    else
        return EOS::NaN;
}

double UsUpSplineParams::d2eref(double V)
{
    if( V < V0 )
        return 0.5*(V0-V)*d2Pref(V)-dPref(V);
    else if( V <= Vmax )
    {
        double eta = 1 - V/V0;
        return K0/V0*(1+2*FD0*eta);       // d2eref = -dPref
    }
    else
        return EOS::NaN;
}

double UsUpSplineParams::Tref(double V)
{
    if( V < V0 )
    {
        double Tv[1];
        int stat;
        if( (stat=ODE::Integrate(V,Tv,NULL)) )
        {
    	    /*************
            EOSerror->Log("UsUpSplineParams::Tref",__FILE__,__LINE__,this,
                          "ODE.Integrate failed with status: %s\n",
    	    	          ODE::ErrorStatus(stat) );
            *************/
            return EOS::NaN;
        }
        return exp(-intGoV(V))*(T0 + 0.5*Tv[0]/Cv);
    }
    else if( V <= Vmax )
        return T0*exp(-intGoV(V));
    else
        return EOS::NaN;
}

int UsUpSplineParams::F(double *yp, const double *y, double V)
{
    if( V/V0 <= 1 - 1/s1 )
    {
        //cerr << "V/V0=" << V/V0 << ",  1-1/s1=" << 1-1/s1 << "\n";
        return 1;
    }
    yp[0] = (dPref(V)*(V0-V) + Pref(V)-P0)*exp(intGoV(V));
    return 0;
}

double UsUpSplineParams::GoV(double V)
{
    return (G0-G1)/V + G1/V0;
}

double UsUpSplineParams::dGoV(double V)
{
    return -(G0-G1)/sqr(V);
}

double UsUpSplineParams::d2GoV(double V)
{
    return 2*(G0-G1)/(V*V*V);
}

double UsUpSplineParams::intGoV(double V)
{
    return (G0-G1)*log(V/V0) + (G1/V0)*(V-V0);
}

double UsUpSplineParams::rhoc2ref(double V)
{
    double eta = 1-V/V0;
    if( eta > 0 )
    {
    // (rho*c)^2 = -(d/dV)Pref +(Gamma/V)*(Pref+(d/dV)eref)
    // Hugoniot reference curve
    // eref = e0 + 0.5*(Pref+P0)*(V0-V)
        double uinv[3];
        if( Us_inv.Evaluate(eta, uinv) )
            return EOS::NaN;
        double us = 1/uinv[0];
        return sqr(us/V0)*(1+eta*us*uinv[1]*(GoV(V)*V0*eta-2));
    }
    else if( V <= Vmax )
        return K0/V0*(1+2*FD0*eta); // -dPref/dV
    else
        return EOS::NaN;
}

double UsUpSplineParams::drhoc2ref(double V)
{
    double eta = 1-V/V0;
    if( eta > 0 )
    {
    // (rho*c)^2 = -(d/dV)Pref +(Gamma/V)*(Pref+(d/dV)eref)
    // (d/dV)(rho*c)^2 = -d2Pref(V)+dGoV(V)*(Pref(V)+deref(V))
    //                   + GoV(V)*(dPref(V)+d2eref(V))
    // Hugoniot reference curve
    // eref = e0 + 0.5*(Pref+P0)*(V0-V)
        double uinv[3];
        if( Us_inv.Evaluate(eta, uinv) )
            return EOS::NaN;
        double us   = 1/uinv[0];
        double fpof = uinv[1]/uinv[0];
        double dv   = V0 - V;
        double dp   = -(1.-2*eta*fpof);
        double d2p  = (-2/V0)*(fpof*(2.-3*eta*fpof)+eta*us*uinv[2]);
        return sqr(us/V0)*((0.5*GoV(V)*dv-1)*d2p+0.5*dGoV(V)*dv*(dp+1));
    }
    else if( V <= Vmax )
        return -2*FD0*K0/(V0*V0); // -d2Pref
    else
        return EOS::NaN;
}

// Hugoniot reference curve

double UsUpSplineParams::Us(double eta) const
{
    return 1./ Us_inv.Evaluate(eta);   
}

int UsUpSplineParams::UsInv(double eta, double uinv[3]) const
{
    return Us_inv.Evaluate(eta, uinv);
}

int UsUpSplineParams::Legendre(double eta, double &c, double &s) const
{
    double uinv[3];
    if( Us_inv.Evaluate(eta, uinv) )
        return 1;
    c = 1./(uinv[0] -eta*uinv[1]);
    s= -uinv[1]*c;
    return 0;
}
