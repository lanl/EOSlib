#include "BKW.h"

double BKW::P(double V, double e)
{
    if( V<=0. )
        return NaN;
    double lnV =log(V/Vunit);
    double lnP = lnPref(lnV);
    double PrefV = Punit*exp(lnP);
    double erefV = eunit*exp(lneref(lnP));
    return PrefV + (Gref(lnV)/V)*(e+de-erefV);  
}

double BKW::e_PV(double p, double V)
{
    if( V<=0. )
        return NaN;
    double lnV =log(V/Vunit);
    double lnP = lnPref(lnV);
    double PrefV = Punit*exp(lnP);
    double erefV = eunit*exp(lneref(lnP));
    return erefV-de + V*(p-PrefV)/Gref(lnV);  
}

double BKW::c2(double V, double e)
{
    if( V <= 0. )
        return NaN;
    double lnV = log(V/Vunit);
    double lnP = lnPref(lnV);
    double  PrefV = Punit*exp(lnP);
    double deref  = e+de-eunit*exp(lneref(lnP));
    double c2refV = V*PrefV*gref(lnV);
    double G  = Gref(lnV);
    double dG = -((12.*Tn[4]*lnV+6.*Tn[3])*lnV+2.*Tn[2]);// dGamma/dlnV
    return c2refV+(G*(G+1.)-dG)*deref;
}

double BKW::FD(double V, double e)
{
    // FD = { cref^2 - 0.5*[V(d/dV)cref^2]
    //        + 0.5*[(G+2)(G(G+1)-dG/dlnV)+d^G/dlnV^2-(2G+1)dG/dlnV][e+de-eref]
    //      } / c^2
    double lnV = log(V/Vunit);
    double lnP = lnPref(lnV);
    double  PrefV = Punit*exp(lnP);
    double deref  = e+de-eunit*exp(lneref(lnP));
    double c2refV = V*PrefV*gref(lnV);
    double G  = Gref(lnV);
    double dG = -((12.*Tn[4]*lnV+6.*Tn[3])*lnV+2.*Tn[2]); // dGamma/dlnV
    double d2G = -(24.*Tn[4]*lnV+6.*Tn[3]);               // (d/dlnV)^2 Gamma
    double cs2 = c2refV+(G*(G+1.)-dG)*deref;

    double  dlnP=gref(lnV);
    double d2lnP=(12.*Pn[4]*lnV+6.*Pn[3])*lnV+2.*Pn[2];
    double dc2refV = V*PrefV*(dlnP*(1.-dlnP)-d2lnP);
    
    return  ( c2refV-0.5*dc2refV+0.5*((G+1.)*((G+2.)*G-3.*dG)+d2G)*deref )/cs2;
}

double BKW::Gamma(double V, double e)
{
    double lnV=log(V/Vunit);
    return Gref(lnV);
}

//

int BKW::NotInDomain(double V, double e)
{
    return V < 0. || P(V,e) < 0. || c2(V,e) <= 0.;
}

// Thermal properties

double BKW::T(double V, double e)
{
    double lnV=log(V/Vunit);
    double lnP=lnPref(lnV);
    double erefV = eunit*exp(lneref(lnP));
    double TrefV = Tunit*exp(lnTref(lnV));
    return TrefV + (e+de-erefV)/Cv;
}

double BKW::S(double V, double e)
{
    double lnV=log(V/Vunit);
    double CTref = Cv*Tunit*exp(lnTref(lnV));
    double lnP=lnPref(lnV);
    double deref = e+de-eunit*exp(lneref(lnP));
    return Scj + Cv*log(1.+deref/CTref);
}

double BKW::CV(double, double)
{
    return Cv;
}

// Reference curve (CJ isentrope)

double BKW::Pref(double V)
{
    double lnV=log(V/Vunit);
    return Punit*exp(lnPref(lnV));
}

double BKW::eref(double V)
{
    double lnV=log(V/Vunit);
    double lnP=lnPref(lnV);
    return eunit*exp(lneref(lnP));    
}

double BKW::Tref(double V)
{
    double lnV=log(V/Vunit);
    return Tunit*exp(lnTref(lnV));
}

double BKW::c2ref(double V)
{   // -V^2 *(d/dV)Pref = -V*(d/dlnV)Pref
    double lnV=log(V/Vunit);
    double lnP=lnPref(lnV);
    return V*Punit*exp(lnP)*gref(lnV);
}

double BKW::dc2ref(double V)
{   //  (d/dlnV)c2ref = (d/dlnV)c2ref
    double   lnV=log(V/Vunit);
    double   lnP=lnPref(lnV);
    double  dlnP=gref(lnV);
    double d2lnP=(12.*Pn[4]*lnV+6.*Pn[3])*lnV+2.*Pn[2];
    return V*Punit*(dlnP*(1.-dlnP)-d2lnP)*exp(lnP);
}
