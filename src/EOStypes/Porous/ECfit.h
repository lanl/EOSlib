#ifndef EOSLIB_ECFIT_H
#define EOSLIB_ECFIT_H

#include <OneDFunction.h>

class ECfit : private OneDFunction
{
private:
    double *A;
    double f(double TMD);                       // OneDFunction::f
    ECfit();                                    // disallowed
public:
    double TMD_0;
    double TMD_1;
    
    double beta_0;
    double beta_1;
    
    ECfit(double *a, double TMD0, double TMD1) : A(a)
    {
        TMD_0 = TMD0;
        TMD_1 = TMD1;
        
        beta_0 = f(TMD_0);                      // minimum
        beta_1 = f(TMD_1);                      // maximum
    }
    ~ECfit() {}
    
    double beta(double TMD);
    double TMD(double p);
};

#endif // EOSLIB_ECFIT_H
