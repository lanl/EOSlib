#include "PhiEqSpline.h"

PhiEq_spline::PhiEq_spline(const char *n )
    : PhiEq(n), m(3), ODE(1, 512), Espline(this), s_e(1.)
{
    // derived class constructor must call Initialize
}

PhiEq_spline::~PhiEq_spline()
{
    // NULL
}

double PhiEq_spline::fp(double PV)
{
    double phi0=f(PV);
    double dPV = 0.01*(PV_smax-PV_min);
    
    int count;
    for(count=20; count-->0; dPV *= 0.25 )
    { 
        double phi1 = f(PV+dPV);
        double phi2 = f(PV+2*dPV);    

        double dphi = 0.5*(phi2-phi0);
        double d2phi = (phi2-2*phi1+phi0);
        if( abs(d2phi) < 0.01*dphi )
            return (dphi-d2phi)/dPV;
    }
    return NaN;
}


double PhiEq_spline::phi(double PV)
{
    if( PV <= PV_min )
        return phi_min;
    if( PV >= PV_max )
        return phi_max;
    if( PV >= PV_smax )
    { // extrapolation
        double ratio = (PV_max - PV)/(PV_max - PV_smax);
        return phi_max - (phi_max-phi_smax)*pow(ratio,m);
    }
        
    return SplineFunction::Evaluate(PV/s_e);
}


int PhiEq_spline::phi(double PV, double phi[3])
{
    if( PV <= PV_min )
        PV = PV_min;

    if( PV <= PV_smax )
    {
        int status_p = SplineFunction::Evaluate(PV/s_e, phi);
        phi[1] *= s_e;
        phi[2] *= sqr(s_e);
        
        return status_p;
    }
    
    PV = min(PV_max,PV);    
    double ratio = (PV_max - PV)/(PV_max - PV_smax);
    phi[0] = phi_max - (phi_max-phi_smax)*pow(ratio,m);
    double dP = PV_max - PV_smax;
    phi[1] = m*(phi_max-phi_smax)*pow(ratio,m-1)/dP;
    phi[2] = m*(m-1)*(phi_max-phi_smax)*pow(ratio,m-2)/sqr(dP);

    return 0;
}

double PhiEq_spline::PV(double phi)
{
    if( phi <= phi_min )
        return PV_min;
    if( phi <= phi_smax )
        return s_e*SplineFunction::Inverse(phi);
    
    phi = min(phi_max,phi);
    double ratio = (phi_max-phi)/(phi_max-phi_smax);
    return PV_max - (PV_max - PV_smax)*pow(ratio,1/m);
}



int PhiEq_spline::F(double *yp, const double *, double phi)
{
    yp[0] = PV(phi)/phi;
    return  isnan(yp[0]);
}

double PhiEq_spline::Energy(double phi)
{
    double e[1];
    int status_e = Integrate(phi,e);
    if( status_e )
    {
        EOSstatus = EOSlib::fail;
        return EOSerror->Log("PhiEq_spline::Energy",
                __FILE__, __LINE__, this,
                  "Integrate failed with status %s\n",
               ODE::ErrorStatus(status_e));
    }
    return s_e*e[0];
}

double PhiEq_energy::f(double phi)
{
    return phi_eq->Energy(phi);
}


double PhiEq_spline::energy(double phi)
{
    if( phi <= phi_min )
        return 0.0;
    if( phi >= phi_max )
        return e_max;
    if( phi <= phi_smax )    
        return s_e*Espline.Evaluate(phi);
    return Energy(phi);
}


int PhiEq_spline::energy(double phi, double e[3])
{
    phi = max(phi_min,phi);
    if( phi <= phi_smax )
    {
        int status_e = Espline.Evaluate(phi,e);
        e[0] *= s_e;
        e[1] *= s_e;
        e[2] *= s_e;
        return status_e;
    }

    double PV_phi = PV_max;
    if( phi >= phi_max )
    {
        e[0] = e_max;
        e[1] = PV_max/phi_max;
    }
    else
    {
        e[0] = Energy(phi);
        PV_phi = PV(phi);
        e[1] = PV_phi/phi;
    }
    double dPV = PV_max - PV_smax;
    double ratio = (PV_max - PV_phi)/dPV;
    double dphidPV = m*(phi_max-phi_smax)*pow(ratio,m-1)/dPV;
    e[2] = (phi/(dphidPV+1e-10/PV_max) - PV_phi)/sqr(phi);
    // note, e[1]/e[2] -> 1e-10 as phi -> phi_max

    return 0;    
}
