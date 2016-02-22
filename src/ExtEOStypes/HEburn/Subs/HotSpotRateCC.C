#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;

#include "PTequilibrium.h"
#include "HotSpotRateCC.h"
#include "HotSpotHE_CC.h"
//
double HotSpotRateCC::DLAMBDA = 0.1;      // default for dlambda
double HotSpotRateCC::TOL     = 1.0e-6;   // default for tol
//
//
HotSpotRateCC::HotSpotRateCC() : HErate2("HotSpotRateCC"),
                   dlambda(HotSpotRateCC::DLAMBDA),
                   tol(HotSpotRateCC::TOL)
{
    source_type = 2;    // stiff integrator, no time step constraint
    //
    Pref = 1.0;
    tref = 1.0;
    n = 0.0;
    A = 0.0;
    B = 0.0;
    Pmax = Pign = 0.0;
    fign = fmax =0.0;
    //
    P0 = P1 = 0.0;
    f0 = f1 = 0.0;
    df1 = B2 = 0.0;
    //
    Cr = Pr = C = 0.0;
    fn = 0.0;
    //
    sc = smax = 0.;
    gc = dgc = d2gc = 0.0;
    gf = 1.0;
    //
    Nratio = 0.0;
    eratio = 0.0;
    Q      = 0.0;
    //
    h1 = t1 = h2 = t2 = 0.0;
    tb = a2 = a3 = t3 = 0.0;
}

HotSpotRateCC::~HotSpotRateCC()
{
    // Null
}

HErate2 *HotSpotRateCC::Copy(PTequilibrium *eos)
{
    HotSpotRateCC *rate = new HotSpotRateCC;
    rate->dlambda     = dlambda;  
    rate->tol         = tol;
    rate->source_type = source_type;
    //
    rate->Pref        = Pref;
    rate->tref        = tref;
    rate->n           = n;
    rate->A           = A;
    rate->B           = B;
    //
    rate->Pign        = Pign;
    rate->Pmax        = Pmax;
    rate->fign        = fign;
    rate->fmax        = fmax;
    //
    rate->P0          = P0;
    rate->P1          = P1;
    rate->f0          = f0;
    rate->f1          = f1;
    rate->df1         = df1;
    rate->B2          = B2;
    //
    rate->Cr          = Cr;
    rate->Pr          = Pr;
    rate->C           = C;
    rate->fn          = fn;
    //
    rate->Nratio      = Nratio;
    rate->eratio      = eratio;
    rate->Q           = Q;
    //
    rate->sc          = sc;
    rate->smax        = smax;
    rate->gc          = gc;
    rate->dgc         = dgc;
    rate->d2gc        = d2gc;
    rate->gf          = gf;
    //
    rate->h1          = h1;
    rate->t1          = t1;
    rate->h2          = h2;
    rate->t2          = t2;
    rate->tb          = tb;
    rate->a2          = a2;
    rate->a3          = a3;
    rate->t3          = t3;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *HotSpotRateCC::Idof()
{
    return new HotSpotHE_CC;
}

void HotSpotRateCC::PreInit(Calc &calc)
{
    dlambda     = DLAMBDA;
    tol         = TOL;
    source_type = 2;
    calc.Variable("dlambda",     &dlambda);
    calc.Variable("tol",         &tol);
    calc.Variable("source_type", &source_type);
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
    Pmax = Pign = 0.0;
    fign = fmax = 0.0;
    calc.Variable("Pign", &Pign);
    calc.Variable("Pmax", &Pmax);
    calc.Variable("fmax", &fmax);
    //
    P0  = P1 = 0.0;
    f0  = f1 = 0.0;
    df1 = B2 = 0.0;
    calc.Variable("P0",  &P0);
    calc.Variable("P1",  &P1);
    calc.Variable("df1", &df1);
    //
    C = Cr = Pr = 0.0;
    fn = 0.0;
    calc.Variable("Cr",  &Cr);
    calc.Variable("Pr",  &Pr);
    calc.Variable("fn",  &fn);
    //
    sc = smax = 0.;
    gc = dgc = d2gc = 0.0;
    gf = 1.0;
    calc.Variable("sc", &sc);
    //
    Nratio = 0.0;
    eratio = 0.0;
    Q      = 0.0;
    calc.Variable("Nratio", &Nratio);
    calc.Variable("Q", &Q);
    //
    h1 = t1 = h2 = t2 = 0.0;
    tb = a2 = a3 = t3 = 0.0;
    calc.Variable("h1", &h1);
    calc.Variable("t1", &t1);
    calc.Variable("h2", &h2);
    calc.Variable("t2", &t2);
}

int HotSpotRateCC::PostInit(Calc &, DataBase *)
{
    if( tol <= 0. || tol >= 1.0 )
        return 1;
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
    if( fn > 0.0 )
    {
        if( fn <= 1.0 || df1 <= 0.0 )
            return 1;
        if( P0 < 0.0 )
            P0 = 0.0;
        if( P1 <= P0 )
            return 1;
        if( Cr <= 0.0 || Pr <= 0.0 )
            return 1;
        C  = Cr/pow((Pr-P0)/Pref,fn);
        f1 = C*pow((P1-P0)/Pref,fn);
        B2 = (fn/df1)*Pref/(P1-P0);
    }
    else
    {
        if( B <= 0.0 )
            return 1;
        if( df1 > 0.0 )
        {   // new form for f(Ps)
            if( P0 < 0.0 )
                P0 = 0.0;
            if( P1 <= P0 )
                return 1;
    
            f0 = exp(A+B*P0/Pref);
            f1 = exp(A+B*P1/Pref) - f0*(1.0+B*(P1-P0)/Pref);
            B2 = B/(f1*df1) * (exp(A+B*P1/Pref) - f0);
            //
            Pmax = Pign = 0.0;
            fign = fmax = 0.0;
        }
        else
        {   // old form for f(Ps): depricated
            if( Pign <= 0.0 || Pmax < Pign )
                return 1;
            fign = exp(A+B*Pign/Pref) / tref;
            if( fmax < fign )
                return 1;
        }
    }
    //
    if( sc <= 0. )
        return 1;
    double exp1 = exp(-sc*sc);
    gc    = 1.0 - exp1;
    dgc   = 2.*sc*exp1;
    d2gc  = 2.*(1.-2.*sc*sc)*exp1;
    smax  = sc - dgc/d2gc;
    gf    = 1./(gc-0.5*dgc*dgc/d2gc);
    //
    if( Nratio < 1.0 )
        return 1;
    eratio = 1./cbrt(Nratio);
    if( Q < 0.0 )
        return 1;
    //
    if( t1<=0.0 || t2<=t1 || h1<=0.0 || h2<=h1 || h2>=1.0 )
        return 1;
    a2 = (h2-h1)/(t2-t1);
    tb = t1 -  h1/a2;
    a3 = 0.5*a2*a2/(1.-h2);
    t3 = t2 + 2.*(1.-h2)/a2;
    if( t1 <= h1/a2 )
        return 1;
    return 0;
}

void HotSpotRateCC::PrintParams(ostream &out)
{
    out << "\t dlambda     = " << dlambda     << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t source_type = " << source_type << "\n";
    out << "\t       Pref  = " << Pref        << "\n"
        << "\t       tref  = " << tref        << "\n"
        << "\t       n     = " << n           << "\n";
    if( fn > 0.0 )
    {
        cout << "\t    Cr       = " << Cr     << "\n"
             << "\t    Pr       = " << Pr     << "\n"
             << "\t    P0       = " << P0     << "\n"
             << "\t    P1       = " << P1     << "\n"
             << "\t    fn       = " << fn     << "\n"
             << "\t    df1      = " << df1    << "\n";
    }
    else
    {
        cout << "\t       A     = " << A           << "\n";
        cout << "\t       B     = " << B           << "\n";
        if( df1 > 0.0 )
        {
            cout << "\t    P0       = " << P0     << "\n"
                 << "\t    P1       = " << P1     << "\n"
                 << "\t    df1      = " << df1    << "\n";
        }
        else
        {
            cout << "\t    Pign     = " << Pign   << "\n"
                 << "\t    Pmax     = " << Pmax   << "\n"
                 << "\t    fmax     = " << fmax   << "\n";
        }
    }
    out << "\t       sc    = " << sc          << "\n";
    out << "\t    Nratio   = " << Nratio      << "\n"
        << "\t       Q     = " << Q           << "\n";
    out << "\t      h1     = " << h1          << "\n"
        << "\t      t1     = " << t1          << "\n"
        << "\t      h2     = " << h2          << "\n"
        << "\t      t2     = " << t2          << "\n";
}

int HotSpotRateCC::ConvertParams(Convert &convert)
{
    if( HErate2::ConvertParams(convert) )
        return 1;
    double s_P, s_t, s_e;
    if( !finite(s_P=convert.factor("P"))
        || !finite(s_t=convert.factor("time"))
        || !finite(s_e=convert.factor("e")) )
        return 1;
    Pref  *= s_P;
    tref  *= s_t;
    //
    Pign  *= s_P;
    Pmax  *= s_P;
    //
    P0    *= s_P;
    P1    *= s_P;
    Pr    *= s_P;
    //
    Q     *= s_e;
    t1    *= s_t;
    t2    *= s_t;
    tb    *= s_t;
    t3    *= s_t;
    a2    /= s_t;
    a3    /= (s_t*s_t);
    return 0;    
}
