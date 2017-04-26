#ifndef EOSLIB_SCALED_DN_H
#define EOSLIB_SCALED_DN_H

#include "DnKappa.h"
//
//
#define ScaledDn_vers "ScaledDn_v2.0.1"
#define ScaledDn_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ScaledDn_lib_vers;
extern const char *ScaledDn_lib_date;
//
extern "C" { // DataBase functions
    char *DnKappa_ScaledDn_Init();
    char *DnKappa_ScaledDn_Info();
    void *DnKappa_ScaledDn_Constructor();
}
//
// Ref: ISD 14 (2010) pp 779-788
// DSD Calibration for PBX9502 with T, rho and material lot variations
// Larry Hill and Tariq Aslam
class ScaledDn : public DnKappa
{
    ScaledDn(const ScaledDn&);              // disallowed    
    void operator=(const ScaledDn&);        // disallowed
    ScaledDn *operator &();                 // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
    // Dn(kappa)/Dcj = 1 - ks *(1+c2*ks+c3*ks^2)/(1+c4*ks+c5*ks^2)
    // where ks = lscale*kappa
    //
    // Dcj/Dcj_r = 1 + c6*(rho0/rho0_r-1) - c8*(T0/T0_r-1)
    //
    // lscale/lscale_r = 1+c7*(rho0/rho0_r-1)-c9*(T0/T0_r-1)+c10*(T0/T0_r-1)^2
    double c2;
    double c3;
    double c4;
    double c5;
    double c6;
    double c7;
    double c8;
    double c9;
    double c10;
    double Dcj_r;
    double lscale_r;
    double rho0_r;
    double T0_r;
    //
    double T0;
    double rho0;
    double lscale;
    ScaledDn();
    ~ScaledDn();
    ostream &PrintComponents(ostream &out);
    double Dn(double kappa);
    int InitState(double rho, double T);
};

#endif // EOSLIB_SCALED_DN_H
