#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "HotSpotCC_Rate.h"
#include "HotSpotCC_HE.h"
//
double HotSpotCC_Rate::DLAMBDA = 0.1;      // default for dlambda
double HotSpotCC_Rate::TOL     = 1.0e-6;   // default for tol
//
//
HotSpotCC_Rate::HotSpotCC_Rate() : HErate("HotSpotCC_Rate"),
                   dlambda(HotSpotCC_Rate::DLAMBDA),
                   tol(HotSpotCC_Rate::TOL)
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

HotSpotCC_Rate::~HotSpotCC_Rate()
{
    // Null
}

HErate *HotSpotCC_Rate::Copy(PTequilibrium *eos)
{
    HotSpotCC_Rate *rate = new HotSpotCC_Rate;
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

IDOF *HotSpotCC_Rate::Idof()
{
    return new HotSpotCC_HE;
}

void HotSpotCC_Rate::PreInit(Calc &calc)
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
    fign = fmax =0.0;
    calc.Variable("Pign", &Pign);
    calc.Variable("Pmax", &Pmax);
    calc.Variable("fmax", &fmax);
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

int HotSpotCC_Rate::PostInit(Calc &, DataBase *)
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
    if( B <= 0.0 )
        return 1;
    if( Pign <= 0.0 || Pmax < Pign )
        return 1;
    fign = exp(A+B*Pign/Pref) / tref;
    if( fmax < fign )
        return 1;
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

void HotSpotCC_Rate::PrintParams(ostream &out)
{
    out << "\t dlambda     = " << dlambda     << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t source_type = " << source_type << "\n";
    out << "\t       Pref  = " << Pref        << "\n"
        << "\t       tref  = " << tref        << "\n"
        << "\t       n     = " << n           << "\n"
        << "\t       A     = " << A           << "\n"
        << "\t       B     = " << B           << "\n"
        << "\t    Pign     = " << Pign        << "\n"
        << "\t    Pmax     = " << Pmax        << "\n"
        << "\t    fmax     = " << fmax        << "\n";
    out << "\t       sc    = " << sc          << "\n";
    out << "\t    Nratio   = " << Nratio      << "\n"
        << "\t       Q     = " << Q           << "\n";
    out << "\t      h1     = " << h1          << "\n"
        << "\t      t1     = " << t1          << "\n"
        << "\t      h2     = " << h2          << "\n"
        << "\t      t2     = " << t2          << "\n";
}

int HotSpotCC_Rate::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_P, s_t, s_e;
    if( !finite(s_P=convert.factor("P"))
        || !finite(s_t=convert.factor("time"))
        || !finite(s_e=convert.factor("e")) )
        return 1;
    Pref  *= s_P;
    tref  *= s_t;
    Pign  *= s_P;
    Pmax  *= s_P;
    Q     *= s_e;
    t1    *= s_t;
    t2    *= s_t;
    tb    *= s_t;
    t3    *= s_t;
    a2    /= s_t;
    a3    /= (s_t*s_t);
    return 0;    
}
