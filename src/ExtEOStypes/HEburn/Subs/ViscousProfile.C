#include <iostream>
#include <iomanip>
using namespace std;

#include "ViscousProfile.h"

double ViscousProfile::Q(double V, double e, double lambda, double dudx)
{
    double du = dudx*len;
    double c  = FF.c(V,e,lambda); 
    return - nu*(du/V)*(c + r*std::abs(du));
}

ViscousProfile::ViscousProfile(FFrate *rate, double pvn, double umin)
                        : ODE(3,512), FF(*rate), Pvn(pvn), Umin(umin)
{
    reactants = FF.Reactants();
    products  = FF.Products();
     nu = 1.0;
    len = 1.0;
      r = 0.25; 
    ws_0 = FF.hug.ws0;
    if( Umin <= 0.0 )
        Umin = 0.001*ws_0.us;    // scaled to sound speed
    Detonation *det = products->detonation(ws_0,ws_0.P);
    if( det == NULL )
    {
        status = 1;
        return;
    }
    if( det->CJwave(RIGHT,ws_cj) )
    {
        status = 2;
        return;
    }
    D = ws_cj.us;
    m = D / ws_0.V;
    Hydrostate(Umin, 0.0);
    Pmin = p;
    Hugoniot *hug = reactants->shock(ws_0);
    if( hug == NULL )
    {
        status = 2;
        return;
    }
    if( hug->u_s(D,RIGHT,ws_vn) )
    {
        status = 3;
        return;
    }
    if( ws_vn.P > Pvn )
    {
        Pvn = ws_vn.P;
        ws_s = ws_cj;
        ws_w = ws_cj;
    }
    else
    {
        if( hug->P(Pvn,RIGHT,ws_vn) )
        {
            status = 4;
            return;
        }
        D = ws_vn.us;
        if( det->u_s(D,RIGHT,ws_s) )
        {
            status = 5;
            return;
        }
        if( det->w_u_s(D,RIGHT,ws_w) )
        {
            status = 6;
            return;
        }
    }
    m = D / ws_0.V;
    delete hug;
    delete det;
    status = 7;
    // Note: Must call Reinit() to initialize ODE
}
int ViscousProfile::Reinit(double nup, double lenp, double rp)
{
    nu = nup;
    if( lenp > 0. )
        len = lenp;
    if( rp > 0. )
        r = rp;
    return Reinit();
}
int ViscousProfile::Reinit()
{
    if( !(status==0 || status==7) )
    {
        return NOT_INIT;    // ODE status
    }
    y[0] = Umin;    // skip exponential tail of profile
    y[1] = 0.0;     // lambda
    y[2] = 0.0;     // t
    double xi0 = 0.;
    double dxi0 = 0.1*FF.Xpop(FF.hug.CJ.P);
    int ODEstatus = Initialize(xi0, y, -dxi0);
    status = ODEstatus ? -1 : 0;
    return ODEstatus;
}
ViscousProfile::~ViscousProfile()
{
    deleteEOS(reactants);
    deleteEOS(products);
}

int ViscousProfile::F(double *y_prime, const double *y_xi, double xi)
{
    double      u = y_xi[0];
    double lambda = y_xi[1];
    Hydrostate(u,lambda);
    if( std::isnan(p) )
        return 1;
    double c = FF.c(V,e,lambda);
    //
    //double nup = sqr(p/FF.hug.CJ.P)*nu;
    double nup = nu;
    //
    double S = -q*V/(c*c);
    y_prime[0] = 0.5*c/(r*len)*(1.-sqrt(1.+4.*r*std::abs(S)/nup));
    if( S > 0.0 )
        y_prime[0] = -y_prime[0];
    y_prime[1] = - FF.Rate(lambda,p+max(0.,q))/(D-u);
    //y_prime[1] = - FF.Rate(lambda,p)/(D-u);
    y_prime[2] = 1/(D-u);
    return 0;
}
void ViscousProfile::Hydrostate(double u, double lambda)
{
    V  = ws_0.V - u/m;
    e  = ws_0.e +(ws_0.P/m + 0.5*u)*u;
    p  = FF.P(V,e,lambda);
    q  = m*u - (p-ws_0.P);
    rate = FF.Rate(lambda,p+q);
}
class ViscousProfile::lambda_ODE : public ODEfunc
{
public:
    lambda_ODE() {}
    double f(double xi, const double *y, const double *yp);
};
double ViscousProfile::lambda_ODE::f(double, const double *y, const double *)
{
    return -y[1];   // -lambda
}
int ViscousProfile::Lambda(double &lambda, double &xi)
{
    if( status )
        return NOT_INIT;
    lambda_ODE L;
    lambda = -lambda;   // -lambda(xi) is monotonically increasing
    int status = Integrate(L,lambda,xi,y,yp);
    lambda = -lambda;
    if( status )
        return status;
    Hydrostate();
    return 0;    
}

class ViscousProfile::P_ODE : public ODEfunc
{
public:
    ViscousProfile &VP;
    P_ODE(ViscousProfile *vp) : VP(*vp) {}
    double f(double xi, const double *y, const double *yp);
};
double ViscousProfile::P_ODE::f(double, const double *y, const double *)
{
    VP.Hydrostate(y[0],y[1]);
    return -VP.p;
}
int ViscousProfile::P(double &pxi, double &xi)
{
    if( status )
        return NOT_INIT;
    P_ODE L(this);
    pxi = -pxi;   // -p(xi) is monotonically increasing in viscous profile
                  // will fail in region above Rayleigh line
    int status = Integrate(L,pxi,xi,y,yp);
    pxi = -pxi;
    if( status )
        return status;
    Hydrostate();
    return 0;    
}
ostream &ViscousProfile::Header(ostream &out, double pp)
{
    if( pp < ws_0.P )
    {
        xi0 = t0 = 0.0;
         p0 = ws_0.P;
    }
    else
    {
        p0 = pp;
        if( P(p0,xi0) )
        {
            xi0 = t0 = 0.0;
             p0 = ws_0.P;
        }
        else
            t0 = y[2];
    }
    out.setf(ios::left, ios::adjustfield);
    out        << setw(13) << "# xi"
        << " " << setw(13) << "   t"
        << " " << setw(13) << "   u"
        << " " << setw(13) << "   lambda"
        << " " << setw(13) << "   V"
        << " " << setw(13) << "   e"
        << " " << setw(13) << "   P"
        << " " << setw(13) << "   Q"
        << " " << setw(13) << "   Rate";
    out.setf(ios::showpoint);
    out.setf(ios::right, ios::adjustfield);
    out.setf(ios::scientific, ios::floatfield);
    return out;
}
ostream &ViscousProfile::Print(ostream &out, double xi,
                                             double  u, double lambda, double t)
{
    Hydrostate(u,lambda);
    out        << setw(13) << setprecision(6) << xi-xi0
        << " " << setw(13) << setprecision(6) <<  t-t0
        << " " << setw(13) << setprecision(6) << u
        << " " << setw(13) << setprecision(6) << lambda
        << " " << setw(13) << setprecision(6) << V
        << " " << setw(13) << setprecision(6) << e
        << " " << setw(13) << setprecision(6) << p
        << " " << setw(13) << setprecision(6) << q
        << " " << setw(13) << setprecision(6) << rate;
    return out; 
}

ostream &ViscousProfile::Print(ostream &out, double xi)
{
    if( xi > 0.0 )
    {
        xi = 0.0;
        y[0] = y[1] = y[2] = 0.0;
    }
    else if( Integrate(xi,y,yp) )
        return out;
    Hydrostate();
    out        << setw(13) << setprecision(6) << xi-xi0
        << " " << setw(13) << setprecision(6) << y[2]-t0
        << " " << setw(13) << setprecision(6) << y[0]
        << " " << setw(13) << setprecision(6) << y[1]
        << " " << setw(13) << setprecision(6) << V
        << " " << setw(13) << setprecision(6) << e
        << " " << setw(13) << setprecision(6) << p
        << " " << setw(13) << setprecision(6) << q
        << " " << setw(13) << setprecision(6) << rate;
    //double c = FF.c(V,e,y[1]);
    //out << " " << setw(13) << setprecision(6) << y[0]+c;
    return out; 
}
