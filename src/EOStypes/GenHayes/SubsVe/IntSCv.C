#include <LocalMath.h>

#include "IntSCv_ODE.h"

double IntSCv_ODE::MaxNorm(const double *y0, const double *y1,
						   const double*,    const double*)
{
	double Norm = 0;
	
	for(int i=Dimension(); i--; y0++, y1++)
		Norm = max(Norm, abs(*y0 - *y1)/((abs(*y0)+abs(*y1) + eps_abs)));

	return Norm/eps_rel;
}

IntSCv_ODE::IntSCv_ODE(double eps_a, double eps_r)
            : ODE(2,512), eps_abs(eps_a), eps_rel(eps_r)
{
    // Null
}

int IntSCv_ODE::InitODE(double abs_err, double rel_err)
{
    eps_abs = abs_err;
    eps_rel = rel_err;
    double y[2];
    y[0] = 0.0;
    y[1] = 0.0;
    double T0 = 0;
    double dT = 0.1;
    
    return ODE::Initialize(T0, y, dT);
}


int IntSCv_ODE::F(double *yp, const double *, double sT)
{
    if( sT < 0 )
        return 1;

    yp[0] = SCv(sT);
    yp[1] = (sT==0) ? 0.0 : yp[0]/sT;
    return 0;
}


int IntSCv_ODE::IntSCv(double sT, double &value)
{
    double y[2];
    double yp[2];
    
    int status = Integrate(sT, y, yp);
    if( status )
        return status;
    value = y[0];
    return 0;
}

int IntSCv_ODE::IntSCvT(double sT, double &value)
{
    double y[2];
    double yp[2];
    
    int status = Integrate(sT, y, yp);
    if( status )
        return status;
    value = y[1];
    return 0;
}

class InvIntSCv_ODE : public ODEfunc
{
public:
    InvIntSCv_ODE() {}
    double f(double sT, const double *y, const double *yp);
};

double InvIntSCv_ODE::f(double, const double *y, const double *)
{
    return y[0];
}

int IntSCv_ODE::InvIntSCv(double value, double &sT)
{
    double sTp;
    double y[2];
    double yp[2];
    InvIntSCv_ODE InvCv;
    int status = Integrate(InvCv, value, sTp, y, yp);
    if( status )
        return status;
    sT = sTp;
    return 0;
}
