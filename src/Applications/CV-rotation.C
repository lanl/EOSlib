/***********************************************
Rotational specific heat for ideal diatomic gas

ref: Hypersonic and High Temperature Gas Dynamics
     John D. Anderson, p 438, Fig 11.8

Qrot = sum_{J=0}^\infty (2J+1) exp[-J(J+1)y]
  y = Trot/T
  Trot = 0.5*\hbar^2/(k_b I)
  I = moment of inertia

ref: Statistical Physics
     George H. Wannier, p. 215, Eq (11.21), Fig 11.2

 e       = R*T^2 (d/dT) ln(Q)
 e/(R*Trot) = -(dQ/dy) / Q
 Cv      = (d/dT)y * (de/dy)
 Trot*Cv = -y^2 * (de/dy)
 Cv/R    = y^2 * [Q*(d^2 Q/dy2)- (dQ/dy)^2]/Q^2

************************************************/

#include <Arg.h>
#include <LocalMath.h>

using namespace std;

void RotateQ(double yinv, double &e, double &Cv)
{
    // T/Trot = yinv
    double y = 1./max(1e-2, yinv);
    if( y < 1.e-12 )
    {
        e = yinv;
        Cv = 1.;
    }
    else if( y < 0.25 )
    {   // T > 4*Trot
        // ln(Qrot) = -ln(y) + 1/3 y + 1/90 y^2 + O(y^4)
        // need another term
        double lnQ, dlnQ;
        lnQ = -log(y) +y/3. + y*y/90.;
        dlnQ = -1./y + (1./3.) + (2./90.)*y;
        e = -dlnQ;
        Cv = (1. +2./90. *y*y);
        /**
        double Q   = 1./y +(1./3.) + (1./15.)*y;
        double dQ  = -1./sqr(y) + (1./15.);
        double d2Q =  2./(y*y*y);
        e  = - dQ/Q;
        Cv = y*y * (Q*d2Q-sqr(dQ))/sqr(Q);
        **/
    }
    else if( y > 100. )
    {
        e = 0.0;
        Cv = 0.0;
    }
    else
    {
        double Q   = 1.0;
        double dQ  = 0.0;
        double d2Q = 0.0;
        double expj;
        int j;
        int jmax = 10;
        for( j=1; j<=jmax; j++ )
        {
            expj = (2*j+1)*exp(-j*(j+1)*y);
            Q   += expj;
            dQ  -= j*(j+1)*expj;
            d2Q += sqr(j*(j+1))*expj;
        }
        e  = - dQ/Q;
        Cv = y*y * (Q*d2Q-sqr(dQ))/sqr(Q);
    }
}

int main(int, char **argv)
{
    ProgName(*argv);

// process command line arguments
    double yinv_min = 0.;
    double yinv_max = 10.;
    double nsteps = 100;
    while(*++argv)
    {
        GetVar(Tmin,yinv_min);
        GetVar(Tmax,yinv_max);
        GetVar(nsteps,nsteps);
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    cout.setf(ios::right, ios::adjustfield);

    int i;
    double yinv, e, Cv;
    for( i=0; i<=nsteps; i++ )
    {
        yinv = yinv_min + double(i)/double(nsteps) * (yinv_max-yinv_min);
        RotateQ(yinv, e, Cv);
        cout << setw(12) << yinv
             << " " << setw(12) << e
             << " " << setw(12) << Cv
             << "\n";
    }
    return 0;
}
