#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "Arrhenius.h"
#include "HE1.h"
//
double Arrhenius::DLAMBDA = 0.01;     // default for dlambda
double Arrhenius::DLAMBDA_DT = 0.1;   // default for dlambda_dt
double Arrhenius::RMIN = 1.0e-10;     // minimum rate for default T_cutoff
double Arrhenius::TOL  = 1.0e-6;      // default for tol
//
//
Arrhenius::Arrhenius() : HErate("Arrhenius"),
                n(1.0), k(0.0),T_a(0.0),T_cutoff(0.0),T_max(11604.5),
                m(0.0), T_m(1.0),
                dlambda(Arrhenius::DLAMBDA), dlambda_dt(Arrhenius::DLAMBDA_DT),
                tol(Arrhenius::TOL), dt_min(0.0), source_type(3), integrate(1)
{
    // Null
}

Arrhenius::~Arrhenius()
{
    // Null
}

HErate *Arrhenius::Copy(PTequilibrium *eos)
{
    Arrhenius *rate   = new Arrhenius;
    rate->n           = n;
    rate->k           = k;
    rate->T_a         = T_a;
    rate->m           = m;
    rate->T_m         = T_m;
    rate->T_cutoff    = T_cutoff;
    rate->T_max       = T_max;
    rate->dlambda     = dlambda;  
    rate->dlambda_dt  = dlambda_dt;  
    rate->tol         = tol;  
    rate->dt_min      = dt_min;  
    rate->source_type = source_type;;  
    rate->integrate   = integrate;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *Arrhenius::Idof()
{
    return new HE1;
}

void Arrhenius::PreInit(Calc &calc)
{
    n = 1.; // first order
    k = T_a     = 0.0;
    m           = 0.0;
    T_m         = 1.0;
    T_cutoff    = -1.;
    T_max       = 11604.5;      // 1 ev/k in Kelvin
    dlambda     = DLAMBDA;
    dlambda_dt  = DLAMBDA_DT;
    tol         = TOL;
    dt_min      = 0.0;
    source_type = 3;
    integrate   = 1;
    calc.Variable("n",   &n);
    calc.Variable("k",   &k);
    calc.Variable("T_a", &T_a);
    calc.Variable("m",   &m);
    calc.Variable("T_m", &T_m);
    calc.Variable("T_cutoff",    &T_cutoff);
    calc.Variable("T_max",       &T_max);
    calc.Variable("dlambda",     &dlambda);
    calc.Variable("dlambda_dt",  &dlambda_dt);
    calc.Variable("tol",         &tol);
    calc.Variable("dt_min",      &dt_min);
    calc.Variable("source_type", &source_type);
    calc.Variable("integrate",  &integrate);
}

int Arrhenius::PostInit(Calc &, DataBase *)
{
    //if( n<0.0 || n>1. )
    if( n<0.0 )
        return 1;
    if( k <= 0.0 )
        return 1;
    if( T_a <= 0.0 )
        return 1;
    if( T_cutoff < 0.0 )
    {
        T_cutoff = max(-T_a/log(RMIN/k),0.0);
        // ln(Rate(T)/k) = m*ln(T/T_m) -T_a/T
        // one Newton iteration
        double dlnRdT = (T_a/T_cutoff+m)/T_cutoff;
        T_cutoff -= log(Rate(T_cutoff)/RMIN)/dlnRdT;
    }
    if( T_max < 1.1*T_cutoff )
        return 1;
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    if( dlambda_dt<=0. || dlambda_dt>=1. )
        return 1;
    if( tol < 0. )
        return 1;
    if( source_type<1 || 3<source_type )
        return 1;
    return 0;
}

void Arrhenius::PrintParams(ostream &out)
{
    out << "\t      n      = " << n           << "\n"
        << "\t      k      = " << k           << "\n"
        << "\t      T_a    = " << T_a         << "\n"
        << "\t      m      = " << m           << "\n"
        << "\t      T_m    = " << T_m         << "\n"
        << "\t T_cutoff    = " << T_cutoff    << "\n"
        << "\t T_max       = " << T_max       << "\n"
        << "\t dlambda     = " << dlambda     << "\n"
        << "\t dlambda_dt  = " << dlambda_dt  << "\n"
        << "\t tol         = " << tol         << "\n"
        << "\t     dt_min  = " << dt_min      << "\n"
        << "\t source_type = " << source_type << "\n"
        << "\t   integrate = " << integrate   << "\n";
}

int Arrhenius::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_t, s_T;
    if( !finite(s_t=convert.factor("time")) || !finite(s_T=convert.factor("T")) )
        return 1;
    k /= s_t;
    T_a *= s_T;
    T_m *= s_T;
    T_cutoff *= s_T;
    T_max    *= s_T;
    dt_min   *= s_t;
    return 0;    
}
