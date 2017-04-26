#include <LocalMath.h>

#include "ECfit.h"

double ECfit::beta(double TMD)
{
    if( TMD <= TMD_0 )
        return beta_0;
    if( TMD >= TMD_1 )
        return beta_1;
    return f(TMD);

}

double ECfit::TMD(double p)
{
    if( p <= beta_0 )
        return TMD_0;
    if( p >= beta_1 )
        return TMD_1;
    return inverse(p, TMD_0, TMD_1);
}

double ECfit::f(double TMD)
{
    double chi = 2*(TMD-TMD_0)/(1-TMD_0) - 1;
    double sum = A[9];
    
    for(int i = 8; i >= 0; i--)
        sum = chi*sum + A[i];
    
    sum = max(sum, 0.);
        
    return sum * 1e-3;    // GPa
}
