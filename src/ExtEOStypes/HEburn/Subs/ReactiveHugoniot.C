#include "ReactiveHugoniot.h"
//
double ReactiveHugoniot::Us(double u)
{
    return c0+s*u;
}
double ReactiveHugoniot::dUsdUp(double)
{
    return s;
}
//
ReactiveHugoniot::ReactiveHugoniot() : ODE(1,512)
{
    HE = NULL;
    c0 = s = 0.0;
}
ReactiveHugoniot::~ReactiveHugoniot()
{
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
}

int ReactiveHugoniot::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    if( HE->lambda2 != lambda )
    {
        HE->lambda1 = 1.-lambda;
        HE->lambda2 = lambda;
        HE->Vav = -1.;
    }
    return 0;
}
double ReactiveHugoniot::P(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->P(V,e);
}
double ReactiveHugoniot::T(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->T(V,e);
}
double ReactiveHugoniot::c(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->c(V,e);
}
int ReactiveHugoniot::ddlambda(double V,   double e,  double lambda,
                          double &dP, double &dT)
{
    if( set_lambda(lambda) )
        return 1;
    if( HE->ddlambda(V,e,dP,dT) )
        return 1;
    dP = -dP;
    dT = -dT;
    return 0;
}

#define FUNC "ReactiveHugoniot::Init",__FILE__,__LINE__,HE
int ReactiveHugoniot::Init(PTequilibrium *he, double sp)
{
    if( HE )
    {
        EOS *Eos = static_cast<EOS*>(HE);
        deleteEOS(Eos);
        HE = NULL;
    }
    if( he == NULL )
        return 1;
    HE = new PTequilibrium(*he);
    EOS *reactants = HE->eos1;
    EOS *products  = HE->eos2;
    if( reactants==NULL || products==NULL )
        return 1;
    ws0.V  = reactants->V_ref; 
    ws0.e  = reactants->e_ref;
    ws0.u  = 0.0;   // generalize to hs.u;
    ws0.P  = reactants->P(ws0.V,ws0.e);
    ws0.us = reactants->c(ws0.V,ws0.e);
    c0 = ws0.us;
    //
    Detonation *det = products->detonation(ws0,ws0.P);
    if( det == NULL )
    {
        HE->ErrorHandler()->Log(FUNC,"HE.detonation failed\n");
        return 1;
    }
    if( det->CJwave(RIGHT,CJ) )
    {
        HE->ErrorHandler()->Log(FUNC,"det->CJwave failed");
        return 1;
    }
    delete det;
    s = (sp>0.) ? sp : (CJ.us-c0)/CJ.u;
    double s_reactant = 0.5*reactants->FD(ws0.V,ws0.e);
    if( s < s_reactant )
    {
        HE->ErrorHandler()->Log(FUNC,"bad slope for us(up), "
             "s(%.2le) < s_reactant(%.2le)\n", s,s_reactant);
        return 1;
    }
    //
    double u0 = ws0.u;
    double du = 0.01*CJ.u;
    y[0] = 0.0;
    int status;
    if( (status=Initialize(u0, y, du)) )
    {
        HE->ErrorHandler()->Log(FUNC,"Initialize failed with status %s\n",
                          ErrorStatus(status));
        return 1;
    }
    return 0;
}

int ReactiveHugoniot::F(double *y_prime, const double *y, double u)
{
    Hstate(u);  // set ws
    double lambda = y[0];
    double dP, dT;
    if( ddlambda(ws.V,ws.e,lambda,dP,dT) )
        return 1;
    double dPde = HE->Gamma(ws.V,ws.e)/ws.V;
    double dPdV = ws.P*dPde - HE->c2(ws.V,ws.e)/(ws.V*ws.V);
    double udu = (u/ws.us)*dUsdUp(u);
    double dVdu = -(1.-udu)*ws0.V/ws.us;
    double dedu = u - dVdu*ws0.P;
    double dPdu = (1.+udu)*ws.us/ws0.V;
    y_prime[0] = (dPdu-dPdV*dVdu-dPde*dedu)/dP;
    return 0;   
}

void ReactiveHugoniot::Hstate(double u)
{
    ws.u = u;
    double us = Us(u);
    ws.V  = (1.-u/us)*ws0.V;
    ws.e  = ws0.e + ws0.P*ws0.V*(u/us) + 0.5*u*u;
    ws.P  = ws0.P + u*us/ws0.V;
    ws.us = us;
}
class ReactiveHugoniot::lambda_ODE : public ODEfunc
{
public:
    lambda_ODE() {}
    double f(double u, const double *y, const double *yp);
};
double ReactiveHugoniot::lambda_ODE::f(double, const double *y, const double *yp)
{
    return y[0];   // lambda
}
int ReactiveHugoniot::Lambda(double &lambda)
{
    lambda_ODE L;
    double u;
    int status = Integrate(L,lambda,u,y,yp);
    if( status )
        return status;
    ws_lambda = lambda;
    Hstate(u);  // set ws
    return 0;    
}
class ReactiveHugoniot::P_ODE : public ODEfunc
{
public:
    ReactiveHugoniot *hug;
    P_ODE(ReactiveHugoniot *h) : hug(h) {}
    double f(double u, const double *y, const double *yp);
};
double ReactiveHugoniot::P_ODE::f(double u, const double *y, const double *yp)
{
    return hug->ws0.P + u*hug->Us(u)/hug->ws0.V;
}
int ReactiveHugoniot::P(double p)
{
    if( p > 1.0001*CJ.P )
        return 1;
    if( p > 0.9999*CJ.P )
    {
        ws = CJ;
        ws_lambda = 1.0;
        return 0;
    }
    P_ODE L(this);
    double u;
    int status = Integrate(L,p,u,y,yp);
    if( status )
        return status;
    ws_lambda = y[0];        
    Hstate(u);  // set ws
    return 0;    
}
int ReactiveHugoniot::U(double u)
{
    if( u > 1.0001*CJ.u )
        return 1;
    if( u > 0.9999*CJ.u )
    {
        ws = CJ;
        ws_lambda = 1.0;
        return 0;
    }
    int status = Integrate(u,y,yp);
    if( status )
        return status;
    ws_lambda = y[0];        
    Hstate(u);  // set ws
    return 0;    
}

