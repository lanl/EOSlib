#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "HotSpotRate.h"
#include "HotSpotHE.h"
//
double HotSpotRate::DLAMBDA = 0.1;      // default for dlambda
double HotSpotRate::TOL     = 1.0e-6;   // default for tol
//
//
HotSpotRate::HotSpotRate() : HErate("HotSpotRate"),
                   lambda_burn(1.0),
                   dlambda(HotSpotRate::DLAMBDA),
                   tol(HotSpotRate::TOL)
{
    source_type = 2;    // stiff integrator, no time step constraint
    smax = 0.0;
    //
    Pref = 1.0;
    tref = 1.0;
    n   = 0.0;
    A   = 0.0;
    B   = 0.0;
    P0  = -1.;
    P1  = -1.;
    df1 = 0.0;
    //
    fp0 = 0.0;
    f1  = 0.0;
    B2  = 0.0;
    //
    s1 = s2 = 1000.;
    g1 = dg1 = d2g1 = 0.0;
    gf = 1.0;
}

HotSpotRate::~HotSpotRate()
{
    // Null
}

HErate *HotSpotRate::Copy(PTequilibrium *eos)
{
    HotSpotRate *rate = new HotSpotRate;
    rate->lambda_burn = lambda_burn;
    rate->dlambda     = dlambda;  
    rate->tol         = tol;
    rate->source_type = source_type;
    rate->smax        = smax;
    //
    rate->Pref        = Pref;
    rate->tref        = tref;
    rate->n           = n;
    rate->A           = A;
    rate->B           = B;
    rate->P0          = P0;
    rate->P1          = P1;
    rate->df1         = df1;
    //
    rate->fp0         = fp0;
    rate->f1          = f1;
    rate->B2          = B2;
    //
    rate->s1          = s1;
    rate->g1          = g1;
    rate->dg1         = dg1;
    rate->d2g1        = d2g1;
    rate->gf          = gf;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *HotSpotRate::Idof()
{
    return new HotSpotHE;
}

void HotSpotRate::PreInit(Calc &calc)
{
    lambda_burn = 1.;
    dlambda     = DLAMBDA;
    tol         = TOL;
    source_type = 2;
    smax        = 0.0;
    s1          = 0.0;
    calc.Variable("lambda_burn", &lambda_burn);
    calc.Variable("dlambda",     &dlambda);
    calc.Variable("tol",         &tol);
    calc.Variable("source_type", &source_type);
    calc.Variable("smax",        &smax);
    calc.Variable("s1",          &s1);    
    Pref = 1.0;
    tref = 1.0;
    n    = 0.0;
    calc.Variable("Pref", &Pref);
    calc.Variable("tref", &tref);
    calc.Variable("n",    &n);
    A = 0.0;
    B = 0.0;
    calc.Variable("A", &A);
    calc.Variable("B", &B);

    P0 = -1.;
    P1 = -1.;
    calc.Variable("P0",    &P0);
    calc.Variable("P1",    &P1);
    calc.Variable("Psmax", &P1);    // backwards compatibility
    df1 = 0.0;
    calc.Variable("df1",  &df1);
}

int HotSpotRate::PostInit(Calc &, DataBase *)
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
    if( Pref <= 0.0 )
        return 1;
    if( tref <= 0.0 )
        return 1;
    if( n < 0.0 )
        return 1;
    if( B <= 0.0 )
        return 1;
    fp0 = (P0<0.0) ? 0.0 : exp(A+B*P0/Pref);
    if( P1 <= 0.0 )
    {
        P1  = 1.e12;    // HUGE
        df1 = 0.0;      // not used
        f1  = 0.0;      // not used
        B2  = 0.0;      // not used
    }
    else
    {
        if( P0 > P1 )
            return 1;
        f1  = exp(A+B*P1/Pref) - fp0*(1.+B*(P1-P0)/Pref);
        if( df1 == 0.0 )
            B2 = 0.0;   // not used
        else
            B2 = (B/std::abs(df1))*(exp(A+B*P1/Pref) - fp0)/f1;
    }
    if( s1 > 0. )
    {
        double exp1 = exp(-s1*s1);
        g1    = 1.0 - exp1;
        dg1   = 2.*s1*exp1;
        d2g1 = 2.*(1.-2.*s1*s1)*exp1;
        s2    = s1 - dg1/d2g1;
        gf    = 1./(g1-0.5*dg1*dg1/d2g1);
        //
        smax = s2;
        lambda_burn = 1.-tol;        
    }
    else
    {
        s1 = s2 = 1000.;
        g1 = dg1 = d2g1 = 0.0;
        gf = 1.0;        
        if( smax <= 0.0 )
        {
            // special case, lambda = 1 - exp(-s*s)
            smax = sqrt(-log(1.-lambda_burn));
        }
        else
        {
            lambda_burn = min(1.,g(smax));
            if( lambda_burn <= 0.0 )
                return 1;
        }
    }
    return 0;
}

void HotSpotRate::PrintParams(ostream &out)
{
    out << "\t lambda_burn = " << lambda_burn << "\n"
        << "\t dlambda     = " << dlambda     << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t source_type = " << source_type << "\n"
        << "\t       smax  = " << smax        << "\n"
        << "\t       s1    = " << s1          << "\n"
        //
        << "\t       Pref  = " << Pref        << "\n"
        << "\t       tref  = " << tref        << "\n"
        << "\t       n     = " << n           << "\n"
        << "\t       A     = " << A           << "\n"
        << "\t       B     = " << B           << "\n"
        << "\t       P0    = " << P0          << "\n"
        << "\t       P1    = " << P1          << "\n"
        << "\t      df1    = " << df1         << "\n";
    out << "\t#      fp0   = " << fp0         << "\n"
        << "\t#      f1    = " << f1          << "\n"
        << "\t#      B2    = " << B2          << "\n";
}

int HotSpotRate::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_P, s_t;
    if( !finite(s_P=convert.factor("P")) || !finite(s_t=convert.factor("time")) )
        return 1;
    Pref  *= s_P;
    tref  *= s_t;
    P0    *= s_P;
    P1    *= s_P;
    return 0;    
}
