#include "BKW_VT.h"

double BKW_VT::P(double V, double T)
{
    if( V<=0. )
        return NaN;    
    return Pref(V) + (Gamma(V,T)/V)*Cv*(T-Tref(V));  
}

double BKW_VT::e(double V, double T)
{
    if( V<=0. )
        return NaN;    
    return eref(V) - de +Cv*(T-Tref(V));
}

double BKW_VT::S(double V, double T)
{
    if( V<=0. )
        return NaN;    
    return Scj + Cv*log(T/Tref(V));
}

double BKW_VT::P_V(double V, double T)
{
    if( V<=0. )
        return NaN;    
    double lnV=log(V/Vunit);
    double  lnP=(((Pn[4]*lnV+Pn[3])*lnV+Pn[2])*lnV+Pn[1])*lnV+Pn[0];
    double dlnP=((4.*Pn[4]*lnV+3.*Pn[3])*lnV+2.*Pn[2])*lnV+Pn[1];
    double  G  = -((4.*Tn[4]*lnV+3.*Tn[3])*lnV+2.*Tn[2])*lnV-Tn[1];
    double dG  = -((12.*Tn[4]*lnV+6.*Tn[3])*lnV+2.*Tn[2]);  // (d/dlnV)Gamma
    double  lnTr =(((Tn[4]*lnV+Tn[3])*lnV+Tn[2])*lnV+Tn[1])*lnV+Tn[0];
    double    Tr = Tunit*exp(lnTr);
    double dlnTr = ((4.*Tn[4]*lnV+3.*Tn[3])*lnV+2.*Tn[2])*lnV+Tn[1]; // (d/dlnV)lnTr
    return Punit*exp(lnP)*dlnP/V +Cv*((T-Tr)*(dG-G)-G*dlnTr*Tr)/sqr(V);
}

double BKW_VT::P_T(double V, double T)
{
    if( V<=0. )
        return NaN;    
    return (Gamma(V,T)/V)*Cv;
}

double BKW_VT::CV(double V, double T)
{
    return Cv;
}

double BKW_VT::Gamma(double V, double T)
{
    if( V<=0. )
        return NaN;    
    double lnV=log(V/Vunit);
    return -((4.*Tn[4]*lnV+3.*Tn[3])*lnV+2.*Tn[2])*lnV-Tn[1];
}

double BKW_VT::c2(double V, double T)
{   // (adiabatic sound speed)^2
    if( V <= 0. )
        return NaN;
    double  lnV = log(V/Vunit);
    double  lnP = (((Pn[4]*lnV+Pn[3])*lnV+Pn[2])*lnV+Pn[1])*lnV+Pn[0];
    double dlnP = ((4.*Pn[4]*lnV+3.*Pn[3])*lnV+2.*Pn[2])*lnV+Pn[1];
    double c2ref = -V*Punit*exp(lnP)*dlnP;
    double G  = -( (4.*Tn[4]*lnV+3.*Tn[3])*lnV+2.*Tn[2])*lnV-Tn[1];
    double dG = -((12.*Tn[4]*lnV+6.*Tn[3])*lnV+2.*Tn[2]);// dGamma/dlnV
    double lnTref=(((Tn[4]*lnV+Tn[3])*lnV+Tn[2])*lnV+Tn[1])*lnV+Tn[0];
    double deref = Cv*(T -Tunit*exp(lnTref));
    return c2ref+(G*(G+1.)-dG)*deref;
}

int BKW_VT::NotInDomain(double V, double T)
{
    return T<0. || P_V(V,T)>0.;
}

// reference curve is isentrope thru CJ state

double BKW_VT::Pref(double V)
{
    double lnV=log(V/Vunit);
    double lnP=(((Pn[4]*lnV+Pn[3])*lnV+Pn[2])*lnV+Pn[1])*lnV+Pn[0];
    return Punit*exp(lnP);
}

double BKW_VT::eref(double V)
{
    double lnV=log(V/Vunit);
    double lnP=(((Pn[4]*lnV+Pn[3])*lnV+Pn[2])*lnV+Pn[1])*lnV+Pn[0];
    double lne=(((en[4]*lnP+en[3])*lnP+en[2])*lnP+en[1])*lnP+en[0];
    return eunit*exp(lne);    
}

double BKW_VT::Tref(double V)
{
    double lnV=log(V/Vunit);
    double lnT=(((Tn[4]*lnV+Tn[3])*lnV+Tn[2])*lnV+Tn[1])*lnV+Tn[0];
    return Tunit*exp(lnT);
}
