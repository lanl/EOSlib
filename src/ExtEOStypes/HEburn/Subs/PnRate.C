#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "PnRate.h"
#include "HE1.h"
//
double PnRate::DLAMBDA = 0.01;     // default for dlambda
double PnRate::DLAMBDA_DT = 0.1;   // default for dlambda_dt
double PnRate::RMIN = 1.0e-10;     // minimum rate for default P_cutoff
double PnRate::TOL  = 1.0e-6;      // default for tol
//
//
PnRate::PnRate() : HErate("PnRate"),
                   nu(0.0), k(0.0),Pcj(0.0),P_cutoff(0.0),lambda_burn(1.0),
                   dlambda(PnRate::DLAMBDA), dlambda_dt(PnRate::DLAMBDA_DT),
                   tol(PnRate::TOL), dt_min(0.0), source_type(3), integrate(1)
{
    // Null
}

PnRate::~PnRate()
{
    // Null
}

HErate *PnRate::Copy(PTequilibrium *eos)
{
    PnRate *rate      = new PnRate;
    rate->nu          = nu;
    rate->k           = k;
    rate->Pcj         = Pcj;
    rate->N           = N;
    rate->P_cutoff    = P_cutoff;
    rate->lambda_burn = lambda_burn;
    rate->dlambda     = dlambda;  
    rate->dlambda_dt  = dlambda_dt;  
    rate->tol         = tol;  
    rate->dt_min      = dt_min;
    rate->source_type = source_type;
    rate->integrate   = integrate;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *PnRate::Idof()
{
    return new HE1;
}

void PnRate::PreInit(Calc &calc)
{
    nu          = 1.; // first order
    k = Pcj     = 0.0;
    P_cutoff    = -1.;
    lambda_burn = 1.;
    N           = 1.;
    dlambda     = DLAMBDA;
    dlambda_dt  = DLAMBDA_DT;
    tol         = TOL;
    dt_min      = 0.0;
    source_type = 3;
    integrate   = 1;
    calc.Variable("nu",         &nu);
    calc.Variable("k",          &k);
    calc.Variable("Pcj",        &Pcj);
    calc.Variable("N",          &N);
    calc.Variable("P_cutoff",   &P_cutoff);
    calc.Variable("lambda_burn",&lambda_burn);
    calc.Variable("dlambda",    &dlambda);
    calc.Variable("dlambda_dt", &dlambda_dt);
    calc.Variable("tol",        &tol);
    calc.Variable("dt_min",     &dt_min);
    calc.Variable("source_type",&source_type);
    calc.Variable("integrate",  &integrate);
}

int PnRate::PostInit(Calc &, DataBase *)
{
    if( nu<0.0 || nu>1. )
        return 1;
    if( k <= 0.0 )
        return 1;
    if( Pcj <= 0.0 )
        return 1;
    if( N < 0.0 )
        return 1;
    if( P_cutoff < 0.0 )
        P_cutoff = Pcj*pow(RMIN/k,1./N);
    if( lambda_burn > 1. )
        lambda_burn = 1.;
    if( lambda_burn < 1. - dlambda )
        lambda_burn = 1. - dlambda;
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    if( dlambda_dt<=0. || dlambda_dt>=1. )
        return 1;
    if( tol < 0. )
        return 1;
    return 0;
}

void PnRate::PrintParams(ostream &out)
{
    out << "\t      nu     = " << nu          << "\n"
        << "\t      k      = " << k           << "\n"
        << "\t      Pcj    = " << Pcj         << "\n"
        << "\t      N      = " << N           << "\n"
        << "\t P_cutoff    = " << P_cutoff    << "\n"
        << "\t lambda_burn = " << lambda_burn << "\n"
        << "\t dlambda     = " << dlambda     << "\n"
        << "\t dlambda_dt  = " << dlambda_dt  << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t     dt_min  = " << dt_min      << "\n"
        << "\t source_type = " << source_type << "\n"
        << "\t   integrate = " << integrate   << "\n";
}

int PnRate::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_t, s_P;
    if( !finite(s_t=convert.factor("time")) || !finite(s_P=convert.factor("P")) )
        return 1;
    k        /= s_t;
    Pcj      *= s_P;
    P_cutoff *= s_P;
    dt_min *= s_t;
    return 0;    
}
