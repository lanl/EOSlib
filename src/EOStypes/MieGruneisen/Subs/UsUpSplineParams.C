#include "UsUpSpline.h"
#include <iostream>
#include <iomanip>
using namespace std;

UsUpSplineParams::UsUpSplineParams() : ODE(1, 512)
{
    V0  = P0  = e0 = T0 = S0 = EOS::NaN;
    s1 = V1 = EOS::NaN;
    G0 = G1 = Cv = EOS::NaN;
    us_data = up_data = NULL;
    n_data = 0;
    dusdup0 = EOS::NaN;
    K0 = FD0 = Vmax = Pmin = EOS::NaN;
}

UsUpSplineParams::~UsUpSplineParams()
{
    delete [] us_data;
    delete [] up_data;
}

int UsUpSplineParams::init(EOSbase::Error *EOSerror)
{
    if( std::isnan(s1) ||  s1 <= 1  )
    {
	    EOSerror->Log("UsUpSplineParams::init",__FILE__,__LINE__,NULL,
                      "bad input: s1=%lf\n", s1);
        return 1;
    }
    V1 = (1-1/s1)*V0;
    if( n_data < 1 || up_data[0] != 0 )
    {
	        EOSerror->Log("UsUpSplineParams::init",__FILE__,__LINE__,NULL,
                      "bad input, n_data<1 or up_data[0] != 0\n");
        return 1;
    }                 
    double *us_inv = new double[n_data+1];
    double *eta    = new double[n_data+1];
    int i;
    for( i=0; i<n_data; i++ )
    {
        us_inv[i] = 1./us_data[i];
        eta[i]    = up_data[i]/us_data[i];
    }
    us_inv[i] = 0.0;
    eta[i] = 1/s1;
    double eta_last = eta[0];
    double us_inv_last = us_inv[0];
    for( i=1; i<n_data; i++ )
    { // consistency check
        if( eta[i] <= eta_last || us_inv[i] >= us_inv_last )
        {
	        EOSerror->Log("UsUpSplineParams::init",__FILE__,__LINE__,NULL,
                          "us or up/us not monotonic at i=%d\n",i);
            delete [] us_inv;
            delete [] eta;
            return 1;
        }
        eta_last = eta[i];
        us_inv_last = us_inv[i];
    }
    double fp_0 = std::isnan(dusdup0) ? dusdup0 : -dusdup0*us_inv[0];
    double fp_1 = EOS::NaN;
    int status = Us_inv.ReInit(n_data+1, eta,us_inv, fp_0,fp_1);
    // Spline::~Spline() will delete eta & us_inv
    // ToDo: Add consistency check, Us_inv monotonically decreasing
    if( status )
    {
	    EOSerror->Log("UsUpSplineParams::init",__FILE__,__LINE__,NULL,
                      "Us_inv.ReInit failed\n");
        return 1;
    }
    double eta0 = 0.0;
    double c0, s0;
    (void) Legendre(eta0,c0,s0);
    K0   = c0*c0/V0;
    FD0  = 2*s0;
    Vmax = (1+0.5/FD0)*V0;
    Pmin = P0 - 0.25*K0/FD0; 
    // initialize ref temperature 
	double Tv[1];
	Tv[0] = 0.0;
    double dV = -0.01*V0/s1;	
	status = ODE::Initialize(V0, Tv, dV);
    if( status && EOSerror )
	    EOSerror->Log("UsUpSplineParams::init",__FILE__,__LINE__,NULL,
                      "ODE.Initialize failed with status: %s\n",
	    	          ODE::ErrorStatus(status) );
    return status != 0;
}

int UsUpSplineParams::copy(const UsUpSplineParams &params)
{
    if( params.n_data < 1 )
        return 1;
    V0 = params.V0;
    P0 = params.P0;
    e0 = params.e0;
    T0 = params.T0;
    S0 = params.S0;
    s1 = params.s1;
    G0 = params.G0;
    G1 = params.G1;
    Cv = params.Cv;
    dusdup0 = params.dusdup0;
    if( n_data != params.n_data )
    {
       n_data  = params.n_data;
       delete [] up_data;
       delete [] us_data;
       up_data = new double[n_data];
       us_data = new double[n_data];
    }
    for( int i=0; i<n_data; i++ )
    {
        up_data[i] = params.up_data[i];
        us_data[i] = params.us_data[i];
    }
	return init(NULL);
}

#define calcvar(var) calc.Variable(#var,&var)
void UsUpSplineParams::InitParams(Calc &calc)
{
    V0 = EOS::NaN;
    e0 = 0.;
    T0 = 300.;
    S0 = 0.;
    P0 = 1e-4;
    s1 = V1 = EOS::NaN;
    G0 = G1 = Cv = EOS::NaN;
    dusdup0 = EOS::NaN;
    K0 = FD0 = Vmax = Pmin = EOS::NaN;

    calcvar(V0);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
    calcvar(e0);
    calcvar(T0);
    calcvar(S0);
    calcvar(P0);
    calcvar(s1);
    calcvar(dusdup0);
    calcvar(G0);
    calcvar(G1);
    calcvar(Cv);
    // ToDo: add variable dimension array to calc
    n_data=0;
    delete [] up_data;
    delete [] us_data;
    up_data = us_data = NULL;
    calc.DynamicArray("up_data",32);
    calc.DynamicArray("us_data",32);
}

void UsUpSplineParams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 << "\n"
	    << "\t"   << setw(10) << "P0 = " << P0
	    << "; \t" << setw(10) << "T0 = " << T0
	    << "; \t" << setw(10) << "S0 = " << S0 << "\n"
	    << "\t"   << setw(10) << "s1 = " << s1  << "\n"
	    << "\t"   << setw(10) << "G0 = " << G0
	    << "; \t" << setw(10) << "G1 = " << G1  << "\n"
	    << "\t" << setw(10) << "Cv = " << Cv << "\n";
    if( n_data > 0 )
    {
        if( !std::isnan(dusdup0) )
            out << "\t"   << setw(10) << "dusdup0 = " << dusdup0 << "\n";
        int i;
        out << "\t#n_data = " << n_data << "\n";
        for( i=0; i<n_data; i+=5 )
        {
            int j;
            int jmax = min(i+5,n_data);
            out << "\tup_data(" << i+1 << ") = " << setw(10) << up_data[i];
            for( j=i+1; j<jmax; j++ )
            {
                out << ", " << setw(10) << up_data[j];
            }
            out << "\n";
            out << "\tus_data(" << i+1 << ") = " << setw(10) << us_data[i];
            for( j=i+1; j<jmax; j++ )
            {
                out << ", " << setw(10) << us_data[j];
            }
            out << "\n";
        }
        double c,s;
        Legendre(1/s1,c,s);
        out << "\t#c=" << c << ", s1=" << s << "\n";
    }
    out << "\t#Vmin=" << V1 << "\n";    
    out << "\t#K0=" << K0 << ", FD0=" << FD0 << "\n";    
    out << "\t#Vmax=" << Vmax << ", Pmin=" << Pmin << "\n";    
	out.setf(old, ios::adjustfield);  
}

int UsUpSplineParams::ConvertParams(Convert &convert, EOSbase::Error *EOSerror)
{
	double s_V, s_e, s_u, s_T;
	if(  !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	        || !finite(s_u = convert.factor("u"))
	           || !finite(s_T = convert.factor("T")) )
	{
	    EOSerror->Log("UsUpSplineParams::ConvertParams",__FILE__,__LINE__,NULL,
                      "failed\n");
	    return 1;
	}
	
	V0 *= s_V;
	e0 *= s_e;
	P0 *= s_e/s_V;
	T0 *= s_T;
	S0 *= s_e/s_T;
	Cv *= s_e/s_T;
    for( int i=0; i < n_data; i++ )
    {
        up_data[i] *= s_u;
        us_data[i] *= s_u;
    }
	return init(EOSerror);
}

#define FUNC "UsUpSplineParams::ParamsOK",__FILE__,__LINE__,NULL
int UsUpSplineParams::ParamsOK(Calc &calc, EOSbase::Error *EOSerror)
{
  if( std::isnan(V0) || std::isnan(e0) || std::isnan(P0) || std::isnan(T0) || std::isnan(S0) )
    {
        EOSerror->Log(FUNC,"variable not set: "
                           "V0=%lf, e0=%lf, P0=%lf, T0=%lf, S0=%lf\n",
                           V0, e0, P0, T0, S0);
        return 1;
    }
  if( std::isnan(G0) || std::isnan(G1) || std::isnan(Cv) )
    {
        EOSerror->Log(FUNC,"variable not set: G0=%lf, G1=%lf, Cv=%lf\n",
                           G0, G1, Cv);
        return 1;
    }
    if( V0 <= 0 || P0 < 0 || T0 < 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: V0=%lf, P0=%lf, T0=%lf\n",
                           V0, P0, T0);
        return 1;
    }
    if( G0 <= 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: G0=%lf, G1=%lf\n", G0, G1);
        return 1;
    }
    if( Cv <= 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: Cv=%lf\n", Cv);
        return 1;
    }
    int usn = calc.FetchDynamicArray("us_data",us_data);
    int upn = calc.FetchDynamicArray("up_data",up_data);
    if( usn != upn )
    {
        EOSerror->Log(FUNC,"us_n != up_n: %d, %d\n", usn, upn);
        return 1;
    }
    n_data = usn;    
    return init(EOSerror);
}
