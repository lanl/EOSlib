#include <EOS_VT.h>
#include "Sesame_VT.h"
#include "SesSubs.h"
#include <OneDFunction.h>

class SesameP: public OneDFunction
{
public:
    SesameP(Sesame_VT *e, double t) : eos(e), T(t) {}
    Sesame_VT *eos;
    double T;
    double f(double V);
};

double SesameP::f(double V)
{
    return eos->P(V,T);
}

double Sesame_VT::V(double p, double T)
{
    if( EOSstatus != EOSlib::good )
        return NaN;

    int j = SESsrch(T, T_grd, T_n);
    if( j < 0 || T_n-1 <= j )
            return NaN;
    
    int one = 1;
    double *Ptable = P_table+j*rho_n;
    int i1 = SESsrch(p, Ptable, rho_n);
    if( i1 < 0 )
        i1 = 0;
    else if( i1 > rho_n-2 )
        i1 = rho_n - 2;
    double rho1 = rho_grd[i1];
    double rho2 = rho_grd[i1+1];
   
    Ptable += rho_n;
    int i2 = SESsrch(p, Ptable, rho_n);
    if( i2 < 0 )
        i2 = 0;
    else if( i2 > rho_n-2 )
        i2 = rho_n - 2;
    
    if( i2 < i1 )
        rho1 = rho_grd[i2];
    else if( i1 < i2 )
        rho2 = rho_grd[i2+1];

    SesameP PT(this,T);
    double v = PT.inverse(p, 1/rho2, 1/rho1);
    if( PT.Status() )
        return ErrorHandler()->Log("Sesame_VT::V", __FILE__, __LINE__, this,
                    "inverse failed with status: %s\n", PT.ErrorStatus() );
    return v;
}
