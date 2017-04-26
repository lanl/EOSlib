#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "HotSpotRate2.h"
#include "HotSpotHE2.h"
//
double HotSpotRate2::DLAMBDA = 0.1;      // default for dlambda
double HotSpotRate2::TOL     = 1.0e-6;   // default for tol
//
//
HotSpotRate2::HotSpotRate2() : HErate("HotSpotRate2"),
                   lambda_burn(1.0),
                   dlambda(HotSpotRate2::DLAMBDA),
                   tol(HotSpotRate2::TOL)
{
    source_type = 2;    // stiff integrator, no time step constraint
    smax = 0.0;
    //
    n = 0.0;
    a = 0.0;
    b = 0.0;
    sigma = 0.0;
    Ti = 0.0;
}

HotSpotRate2::~HotSpotRate2()
{
    // Null
}

HErate *HotSpotRate2::Copy(PTequilibrium *eos)
{
    HotSpotRate2 *rate = new HotSpotRate2;
    rate->lambda_burn = lambda_burn;
    rate->dlambda     = dlambda;  
    rate->tol         = tol;
    rate->source_type = source_type;
    rate->smax        = smax;
    //
    rate->n     = n;
    rate->a     = a;
    rate->b     = b;
    rate->sigma = sigma;
    rate->Ti    = Ti;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *HotSpotRate2::Idof()
{
    return new HotSpotHE2;
}

void HotSpotRate2::PreInit(Calc &calc)
{
    lambda_burn = 1.;
    dlambda     = DLAMBDA;
    tol         = TOL;
    source_type = 2;
    smax        = 0.0;
    calc.Variable("lambda_burn", &lambda_burn);
    calc.Variable("dlambda",     &dlambda);
    calc.Variable("tol",         &tol);
    calc.Variable("source_type", &source_type);
    calc.Variable("smax",        &smax);
    n    = 0.0;
    calc.Variable("n", &n);
    b = 0.0;
    calc.Variable("b", &b);
    a = 0.0;
    sigma = 0.0;
    Ti    = 0.0;
    calc.Variable("a",     &a);
    calc.Variable("sigma", &sigma);
    calc.Variable("Ti",    &Ti);
}

int HotSpotRate2::PostInit(Calc &, DataBase *)
{
    if( tol <= 0. || tol >= 1.0 )
        return 1;
    if( lambda_burn >= 1. )
        lambda_burn = 1.-tol;
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    if( !(source_type==2 || source_type==3) )
        return 1;
    //
    if( n < 0.0 )
        return 1;
    if( b <= 0.0 )
        return 1;
    if( a <= 0.0 )
        return 1;
    if( sigma <= 0.0 )
        return 1;
    if( Ti <= 0.0 )
        return 1;
    if( smax <= 0.0 )
    {
        // special case, lambda = 1 - exp(-s*s)
        smax = sqrt(-log(1.-lambda_burn));
        /***********************
        // compute smax such that g(smax) = lambda_burn
        double s1 = 0.0;
        double s2 = 0.0;
        double lambda1 = g(s1);
        if( lambda1 != 0.0 )
            return 1;       // error
        double lambda2 = 0.0;
        int i;
        // find bounds: g(s1) < lambda_burn < g(s2)
        for( i=20; i>0; --i )
        {
            if( lambda2 >lambda_burn )
                break;
            s1 = s2;
            lambda1 = lambda2;
            double dg = dgds(s1);
            if( dg <= 0.0 )
                return 1;   // error, g not monotonic
            s2 += 2.* (lambda_burn-lambda1)/dg;
            lambda2 = g(s2);                
        }
        if( i <= 0 )
            return 1;   // failed to find bound            
        // refine bounds with bisection
        for( i=20; i>0; --i )
        {
            smax = 0.5*(s1+s2);
            double lambda = g(smax);
            if( lambda > lambda_burn )
            {
                if( lambda-lambda_burn < tol  && lambda<=1. )
                    return 0;
                s2 = smax;
            }
            else
            {
                if( lambda_burn-lambda < tol )
                    return 0;
                s1 = smax;
            }
        }
        return 1;   // error, bisection failed to converge
        *************************/
    }
    else
    {
        lambda_burn = min(1.,g(smax));
        if( lambda_burn <= 0.0 )
            return 1;
    }
    //
    return 0;
}

void HotSpotRate2::PrintParams(ostream &out)
{
    out << "\t lambda_burn = " << lambda_burn << "\n"
        << "\t dlambda     = " << dlambda     << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t source_type = " << source_type << "\n"
        << "\t       smax  = " << smax        << "\n"
        //
        << "\t       n     = " << n           << "\n"
        << "\t       b     = " << b           << "\n"
        << "\t       a     = " << a           << "\n"
        << "\t      sigma  = " << sigma       << "\n"
        << "\t      Ti     = " << Ti          << "\n";
}

int HotSpotRate2::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_P, s_T, s_t;
    if( !finite(s_P=convert.factor("P"))
      || !finite(s_T=convert.factor("T"))
      || !finite(s_t=convert.factor("time")) )
         return 1;
    b     /= s_t;
    a     *= s_T/s_P;
    sigma *= s_T;
    Ti    *= s_T;
    return 0;    
}
