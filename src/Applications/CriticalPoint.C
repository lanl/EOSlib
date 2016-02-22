#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>

class Trajectory : public ODE
{
// (d/dz) T      =     T - b*lambda
// (d/dz) lambda = eps*T
// critical point at (0,0)
public:
    double b;
    double eps;
    Trajectory(double B, double Eps) : ODE(2,512), b(B), eps(Eps) {}
    int F(double *yp, const double *y, double z); // ODE::F 
};

int Trajectory::F(double *yp, const double *y, double z)
{
    double T      = y[0];
    double lambda = y[1];
    yp[0] =     T  - b*lambda;
    yp[1] = eps*T;
    return 0;
}

int main(int, char **argv)
{
    ProgName(*argv);
    double b = 2.;
    double eps = 0.1;
    //
    double r      = 0.0001;
    double theta  = 0.;
    //
    double dz = 0.1;
    int nsteps = 500;
    //
    double T_max = 1;
    double l_max = 0.1;

    while(*++argv)
    {
        GetVar(b,b);
        GetVar(eps,eps);
        //
        GetVar(r,r);
        GetVar(theta,theta);
        //
        GetVar(dz,dz);
        GetVar(nsteps,nsteps);
        //
        GetVar(T_max,T_max);
        GetVar(l_max,l_max);
        //        
        ArgError;
    }
    double T0      = r*cos(PI*theta/180.);
    double lambda0 = r*sin(PI*theta/180.);
    Trajectory CP(b,eps);
    double y[2], yp[2];
    y[0] = T0;
    y[1] = lambda0;
    double z = 0.;
    if( CP.Initialize(z,y,dz) )
        cerr << Error("Initialize failed" ) << Exit;
    int i;
    cout << T0 << " " << lambda0 << "\n";
    while( nsteps-- )
    {
        z += dz;
        if( CP.Integrate(z,y,yp) )
            cerr << Error("Integrate failed" ) << Exit;
        double T      = y[0];
        double lambda = y[1];
        if( abs(T) > T_max || abs(lambda) > l_max )
            break;
        cout << T << " " << lambda << "\n";
    }
    return 0;
}
