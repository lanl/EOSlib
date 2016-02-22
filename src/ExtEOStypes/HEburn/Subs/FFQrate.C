#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "FFQrate.h"
#include "HEQ.h"
//
double FFQrate::DLAMBDA = 0.01;     // default for dlambda
double FFQrate::DLAMBDA_DT = 0.1;   // default for dlambda_dt
double FFQrate::TOL  = 1.0e-6;      // default for tol
//
//
FFQrate::FFQrate() : HErate("FFQrate"),
                   a(0.0), b(0.0), Pref(1.), xref(1.), n(1.),lambda_max(0.95),
                   dlambda(FFQrate::DLAMBDA), dlambda_dt(FFQrate::DLAMBDA_DT),
                   tol(FFQrate::TOL), dt_min(0.0), source_type(3), integrate(1)
{
    // Null
}

FFQrate::~FFQrate()
{
    // Null
}

#define FUNC "FFQrate::InitBurn",__FILE__,__LINE__,HE
int FFQrate::InitBurn(PTequilibrium *eos)
{
    if( HErate::InitBurn(eos) )
        return 1;
    if( hug.Init(HE) )
    {
        HE->ErrorHandler()->Log(FUNC,"hug.Init failed\n");
        return 1;
    }
    int status = hug.Lambda(lambda_max);
    if( status )
    {
        HE->ErrorHandler()->Log(FUNC,"hug.Lambda failed with status: %s\n",
              hug.ErrorStatus(status) );
        return 1;
    }
    Pmax = P(hug.ws.V,hug.ws.e,lambda_max);
    Rmax = Rate(Pmax); 
    return 0;
}

HErate *FFQrate::Copy(PTequilibrium *eos)
{
    FFQrate *rate      = new FFQrate;
    rate->dlambda     = dlambda;  
    rate->dlambda_dt  = dlambda_dt;  
    rate->tol         = tol;
    rate->dt_min      = dt_min;
    rate->source_type = source_type;
    rate->integrate   = integrate;
    rate->a           = a;
    rate->b           = b;
    rate->Pref        = Pref;
    rate->xref        = xref;
    rate->n           = n;
    rate->lambda_max  = lambda_max;
    if( rate->InitBurn(eos) )
    {
        delete rate;
        rate = NULL;
    }
    return rate;
}

IDOF *FFQrate::Idof()
{
    return new HEQ;
}

void FFQrate::PreInit(Calc &calc)
{
    lambda_max = 0.95;
    a = b = 0.0;
    Pref = xref = 1.0;
    n = 1.0;
    dlambda     = DLAMBDA;
    dlambda_dt  = DLAMBDA_DT;
    tol         = TOL;
    dt_min      = 0.0;
    source_type = 3;
    integrate   = 1;
    calc.Variable("lambda_max", &lambda_max);
    calc.Variable("a",          &a);
    calc.Variable("b",          &b);
    calc.Variable("Pref",       &Pref);
    calc.Variable("xref",       &xref);
    calc.Variable("n",          &n);
    calc.Variable("dlambda",    &dlambda);
    calc.Variable("dlambda_dt", &dlambda_dt);
    calc.Variable("tol",        &tol);
    calc.Variable("dt_min",     &dt_min);
    calc.Variable("source_type",&source_type);
    calc.Variable("integrate",  &integrate);
}

int FFQrate::PostInit(Calc &, DataBase *)
{
    if( a <= 0.0 || b <= 0.0 )
        return 1;
    if( n < 0. || 1. < n )
        return 1;
    return 0;
}

void FFQrate::PrintParams(ostream &out)
{
    out << "\t        a    = " << a           << "\n"
        << "\t        b    = " << b           << "\n"
        << "\t        Pref = " << Pref        << "\n"
        << "\t        xref = " << xref        << "\n"
        << "\t           n = " << n           << "\n"
        << "\t  lambda_max = " << lambda_max  << "\n"
        << "\t dlambda     = " << dlambda     << "\n"
        << "\t dlambda_dt  = " << dlambda_dt  << "\n"
        << "\t        tol  = " << tol         << "\n"
        << "\t     dt_min  = " << dt_min      << "\n"
        << "\t source_type = " << source_type << "\n"
        << "\t   integrate = " << integrate   << "\n";
}

int FFQrate::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_x, s_t, s_P;
    if( !finite(s_x=convert.factor("L")) || !finite(s_t=convert.factor("time"))
            || !finite(s_P=convert.factor("P")) )
        return 1;
    xref   *= s_x;
    Pref   *= s_P;
    dt_min *= s_t;
    return InitBurn(HE);
}
