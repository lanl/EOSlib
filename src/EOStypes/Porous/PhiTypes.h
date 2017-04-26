#ifndef EOSLIB_PHI_TYPES_H
#define EOSLIB_PHI_TYPES_H

#include "PhiEqSpline.h"

//

extern "C" { // DataBase functions
    char *PhiEq_Phi0_Init();
    char *PhiEq_Phi0_Info();
    void *PhiEq_Phi0_Constructor();
    
    char *PhiEq_Phi1_Init();
    char *PhiEq_Phi1_Info();
    void *PhiEq_Phi1_Constructor();
    
    char *PhiEq_EC_Init();
    char *PhiEq_EC_Info();
    void *PhiEq_EC_Constructor();
}

class Phi0;
class Phi1;
class EC;

//


class Phi0 : public PhiEq_spline
{
private:
    int scale;
    
    Phi0(const Phi0&);                          // disallowed    
    void operator=(const Phi0&);                // disallowed
    Phi0 *operator &();                         // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    double P0;
    double V0;
    
    double phi0;
    double Pc;
    
    double PV_0;
    double PV_1;

    Phi0();
    ~Phi0();
    
    int InitPV()                        { return InitPV(P0*V0, 10*Pc*V0); }
    int InitPV(double pv_1)             { return InitPV(P0*V0, pv_1);     }
    int InitPV(double pv_0, double pv_1);
 
    double f(double PV);                        // PhiEq_spline::f
};

class Phi1 : public PhiEq_spline
{
private:
    Phi1(const Phi1&);                          // disallowed    
    void operator=(const Phi1&);                // disallowed
    Phi1 *operator &();                         // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    double P0;
    double V0;
    
    double PV0, PV1;
    
    double phi0, phi1;
    double dphidP0, dphidP1;
    double a, n;
    
    // scale phi and PV;
    double sphi0;
    double sPV1;
    
    Phi1();
    ~Phi1();

    int InitPV(double PV_0 = NaN, double PV_1 = NaN);
    double f(double sPV);                        // PhiEq_spline::f
};

class EC : public PhiEq_spline
{
private:
    EC(const EC&);                              // disallowed    
    void operator=(const EC&);                  // disallowed
    EC *operator &();                           // disallowed, use Duplicate()
    
    char *expt;
    char *hmx;
    Spline *data;
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    //int ConvertParams(Convert &convert);    
public:
    EC();
    ~EC();

    ostream &PrintEnergy(ostream &out);
    double f(double PV);                        // PhiEq_spline::f
};

#endif // EOSLIB_PHI_TYPES_H
