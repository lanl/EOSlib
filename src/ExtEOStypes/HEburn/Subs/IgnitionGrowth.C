#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "IgnitionGrowth.h"
#include "HE1.h"

double IgnitionGrowth::DLAMBDA = 0.01;     // default for dlambda
double IgnitionGrowth::DLAMBDA_DT = 0.1;   // default for dlambda_dt
double IgnitionGrowth::TOL  = 1.0e-6;      // default for tol


IgnitionGrowth::IgnitionGrowth() : HErate("IgnitionGrowth"),
                   dlambda(IgnitionGrowth::DLAMBDA),
                   dlambda_dt(IgnitionGrowth::DLAMBDA_DT),
                   tol(IgnitionGrowth::TOL)
{
    rho0 = NaN;
    I = a = b = x = NaN;
    lambda_igmax = 0.;
    G1 = c = d = y = NaN;
    lambda_G1max = 0.;
    G2 = e = g = zp = NaN;
    lambda_G2min = 1.;
}

IgnitionGrowth::~IgnitionGrowth()
{
    // Null
}

IgnitionGrowth::IgnitionGrowth(const IgnitionGrowth &IG)
                 : HErate("IgnitionGrowth")
{
    EOSbase::Copy(IG);
    *this = IG;
}

void  IgnitionGrowth::operator=(const IgnitionGrowth &IG)
{
    dlambda      = IG.dlambda;
    dlambda_dt   = IG.dlambda_dt;
    tol          = IG.tol;
    rho0         = IG.rho0;
    I            = IG.I;
    a            = IG.a;
    b            = IG.b;
    x            = IG.x;
    lambda_igmax = IG.lambda_igmax;
    G1           = IG.G1;
    c            = IG.c;
    d            = IG.d;
    y            = IG.y;
    lambda_G1max = IG.lambda_G1max;
    G2           = IG.G2;
    e            = IG.e;
    g            = IG.g;
    zp           = IG.zp;
    lambda_G2min = IG.lambda_G2min;
    InitBurn(IG.HE);
}


HErate *IgnitionGrowth::Copy(PTequilibrium *eos)
{
    IgnitionGrowth *rate = new IgnitionGrowth(*this);
    return rate;
}

IDOF *IgnitionGrowth::Idof()
{
    return new HE1;
}

void IgnitionGrowth::PreInit(Calc &calc)
{
    dlambda    = DLAMBDA;
    dlambda_dt = DLAMBDA_DT;
    tol        = TOL;
    calc.Variable("dlambda",   &dlambda);
    calc.Variable("dlambda_dt",&dlambda_dt);
    calc.Variable("tol",       &tol);
    rho0 = NaN;
    I = a = b = x = NaN;
    lambda_igmax = 0.;
    G1 = c = d = y = NaN;
    lambda_G1max = 0.;
    G2 = e = g = zp = NaN;
    lambda_G2min = 1.;
    calc.Variable("rho0", &rho0);    
    calc.Variable("I", &I);    
    calc.Variable("a", &a);    
    calc.Variable("b", &b);    
    calc.Variable("x", &x);    
    calc.Variable("lambda_igmax", &lambda_igmax);    
    calc.Variable("G1", &G1);    
    calc.Variable("c",  &c);    
    calc.Variable("d",  &d);    
    calc.Variable("y",  &y);    
    calc.Variable("lambda_G1max", &lambda_G1max);    
    calc.Variable("G2", &G2);    
    calc.Variable("e",  &e);    
    calc.Variable("g",  &g);    
    calc.Variable("z",  &zp);    
    calc.Variable("lambda_G2min", &lambda_G2min);    
}

int IgnitionGrowth::PostInit(Calc &, DataBase *)
{
    if( lambda_igmax<0.0 || lambda_igmax>1. )
        return 1;
    if( lambda_G1max<0.0 || lambda_G1max>1. )
        return 1;
    if( lambda_G2min<0.0 || lambda_G2min>1. )
        return 1;
    if( a<0.0 || a>1. )
        return 1;
    if( std::isnan(I) || I<=0. )
        return 1;
    if( std::isnan(b) || b<0.0 || 1 < b )
        return 1;
    if( std::isnan(x) || x<0. )
        return 1;
    if( std::isnan(G1) || G1<=0. )
        return 1;
    if( std::isnan(c) || c<0.0 || 1.<c )
        return 1;
    if( std::isnan(d) || d<0.0 || 1.<d )
        return 1;
    if( std::isnan(y) || y<0.)
        return 1;
    if( std::isnan(G2) || G2<=0. )
        return 1;
    if( std::isnan(e) || e<0.0 || 1.<e )
        return 1;
    if( std::isnan(g) || g<0.0 || 1.<g )
        return 1;
    if( std::isnan(zp) || zp<0. )
        return 1;
    //
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    if( dlambda_dt<=0. || dlambda_dt>=1. )
        return 1;
    if( tol < 0. )
        return 1;
    return 0;
}

void IgnitionGrowth::PrintParams(ostream &out)
{
    out << "\t   rho0   = " << rho0  << "\n"
        << "\t      I   = " << I  << "\n"
        << "\t      a   = " << a  << "\n"
        << "\t      b   = " << b  << "\n"
        << "\t      x   = " << x  << "\n"
        << "\t lambda_igmax   = " << lambda_igmax   << "\n"
        << "\t      G1  = " << G1 << "\n"
        << "\t      c   = " << c  << "\n"
        << "\t      d   = " << d  << "\n"
        << "\t      y   = " << y  << "\n"
        << "\t lambda_G1max   = " << lambda_G1max   << "\n"
        << "\t      G2  = " << G2 << "\n"
        << "\t      e   = " << e  << "\n"
        << "\t      g   = " << g  << "\n"
        << "\t      z   = " << zp << "\n"
        << "\t lambda_G2min   = " << lambda_G2min   << "\n"        
        << "\t dlambda    = " << dlambda    << "\n"
        << "\t dlambda_dt = " << dlambda_dt << "\n"
        << "\t tol        = " << tol        << "\n";
}

int IgnitionGrowth::ConvertParams(Convert &convert)
{
    if( HErate::ConvertParams(convert) )
        return 1;
    double s_t,s_P,s_rho;
    if( !finite(s_t=convert.factor("time"))
        || !finite(s_P=convert.factor("P"))
        || !finite(s_rho=convert.factor("rho")) )
            return 1;
    I  /= s_t;
    G1 /= s_t*pow(s_P,y);
    G2 /= s_t*pow(s_P,zp);
    rho0 *= s_rho;
    return 0;    
}
