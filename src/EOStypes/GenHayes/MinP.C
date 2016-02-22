#include <LocalMath.h>
#include <Arg.h>

double K0  = 14.0;
double Kp0 =  9.8;
double P_BM(double eta)
{
    // eta = V0/V
    double eta2 = pow(eta,(2./3.));
    double eta5 = eta*eta2;
    double eta7 = eta5*eta2;
    
    return 1.5*K0*(eta7-eta5)*(1.+0.75*(Kp0-4.)*(eta2-1.));
}
double dP_BM(double eta)
{
    // eta = V0/V
    double eta2 = pow(eta,(2./3.));
    double eta5 = eta*eta2;
    double eta7 = eta5*eta2;
    
    return (K0/eta)*( (3.5*eta7-2.5*eta5)*(1.+0.75*(Kp0-4)*(eta2-1))
                      + 0.75*(eta7-eta5)*(Kp0-4)*eta2 );
}


int main(int, char **argv)
{
	ProgName(*argv);
    int nsteps = 10;
    double eta_min = 0.1;
    double eta_max = 1.0;
	while(*++argv)
	{
		GetVar(K0,K0);
		GetVar(Kp0,Kp0);
		GetVar(eta_min,eta_min);
		GetVar(eta_max,eta_max);
        
		ArgError;
	}
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    double A = (3./4.)*(Kp0-4.);
    double b = 7./2.+ 2.*A;
    double y = 1. + (-b+sqrt(b*b-18.*A) )/(9.*A);
    double eta1 = 1/pow(y,-3./2.);
    double Pmin = P_BM(eta1);
    double dP   = dP_BM(eta1);
    cout << "eta1 = " << eta1
         << ", P = "  << Pmin
         << ", dP = " << dP
         << "\n";

    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double eta = eta_max - double(i)/double(nsteps) * (eta_max-eta_min);
        double p = P_BM(eta);
        double dp = dP_BM(eta);
        cout << eta << "  " << p << "  " << dp << "\n";    
    }

    return 0;
}
