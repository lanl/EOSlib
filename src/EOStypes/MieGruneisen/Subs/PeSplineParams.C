#include <iostream>
#include <iomanip>
using namespace std;

#include "PeSpline.h"

PeSplineParams::PeSplineParams() : ODE(1, 512)
{
    V0 = P0 = e0 = T0 = S0 = EOS::NaN;
    G0 = G1 = G2 = Cv = EOS::NaN;
    n_data = 0;
    P_data = e_data = NULL;
    Vmin = 0.;
    dPdV0 = dedV0 = EOS::NaN;   // natural spline, second derivative = 0
    dPdV1 = dedV1 = EOS::NaN;
    K0 = FD0 = Vmax = Pmin = EOS::NaN;
}

PeSplineParams::~PeSplineParams()
{
    delete [] P_data;
    delete [] e_data;
}

int PeSplineParams::init(EOSbase::Error *EOSerror)
{
    if( EOSerror == NULL )
        EOSerror = EOSbase::ErrorDefault;
    if( Vmin <= 0 || std::isnan(V0) || Vmin >= V0 )
    {
        EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                "bad input, V0 or Vmin\n");
        return 1;
    }                     
    if( n_data < 2  )   // need at least 2 points for spline
    {
        EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                "bad input, n_data<1\n");
        return 1;
    } 
    int i;
    for( i=0; i<n_data-1; i++ )
    {
        if( P_data[i] >= P_data[i+1] || e_data[i] >= e_data[i+1] )
        {
            EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                    "P or e not monotonic at i=%d\n",i);
            return 1;
        }        
    }
    return Reinit(EOSerror);
}
int PeSplineParams::Reinit(EOSbase::Error *EOSerror)
{
    if( EOSerror == NULL )
        EOSerror = EOSbase::ErrorDefault;
    double eta0 = 0.;
    double eta1 = 1.-Vmin/V0;
    double fp_0 = -V0*dPdV0;    // dP/deta_0
    double fp_1 = -V0*dPdV1;    // dP/deta_1
    int status = P.ReInitCopy(n_data, eta0,eta1, P_data, fp_0,fp_1);
    if( status )
    {
        EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                "P.ReInit failed\n");
        return 1;
    }
    fp_0 = -V0*dedV0;           // de/deta_0
    fp_1 = -V0*dedV1;           // de/deta_1
    status = e.ReInitCopy(n_data, eta0,eta1, e_data, fp_0,fp_1);
    if( status )
    {
        EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                "e.ReInit failed\n");
        return 1;
    }
    // assume at V0 ref curve is isentrope to second order
    K0   = P.Evaluate1(0.);         // assume  Pref(V0)+ deref(V0) = 0
    FD0  = 0.5*P.Evaluate2(0.)/K0;  // assume dPref(V0)+d2eref(V0) = 0
    Vmax = (1+0.5/FD0)*V0;
    Pmin = P0 - 0.25*K0/FD0; 
    // initialize ref temperature 
    double Tv[1] = {0.0};   // ODE error tolerance has problem with y=0
    double dV = -0.1*(V0-Vmin);	
    status = ODE::Initialize(V0, Tv, dV);
    if( status && EOSerror )
        EOSerror->Log("PeSplineParams::init",__FILE__,__LINE__,NULL,
                "ODE.Initialize failed with status: %s\n",
                ODE::ErrorStatus(status) );
    return status != 0;
}

int PeSplineParams::copy(const PeSplineParams &params)
{
    if( params.n_data < 2 )
        return 1;
    V0 = params.V0;
    P0 = params.P0;
    e0 = params.e0;
    T0 = params.T0;
    S0 = params.S0;
    G0 = params.G0;
    G1 = params.G1;
    G2 = params.G2;
    Cv = params.Cv;
    Vmin  = params.Vmin;
    dPdV0 = params.dPdV0;
    dPdV1 = params.dPdV1;
    dedV0 = params.dedV0;
    dedV1 = params.dedV1;
    if( n_data != params.n_data )
    {
        delete [] P_data;
        delete [] e_data;
        n_data  = params.n_data;
        P_data = new double[n_data];
        e_data = new double[n_data];
    }
    int i;
    for( i=0; i<n_data; i++ )
    {
        P_data[i] = params.P_data[i];
        e_data[i] = params.e_data[i];
    }
    return Reinit(NULL);
}

#define calcvar(var) calc.Variable(#var,&var)
void PeSplineParams::InitParams(Calc &calc)
{
    V0 = EOS::NaN;
    e0 = 0.;
    T0 = 300.;
    S0 = 0.;
    P0 = 0.;
    G0 = G1 = Cv = EOS::NaN;
    G2 = 0.;
    n_data = 0;
    Vmin = 0.;
    dPdV0 = dedV0 = EOS::NaN;       // natural spline, second derivative = 0
    dPdV1 = dedV1 = EOS::NaN;
    K0 = FD0 = Vmax = Pmin = EOS::NaN;
    delete [] P_data;
    delete [] e_data;
    P_data = e_data = NULL;
    n_data = 0;
    calcvar(V0);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
    calcvar(e0);
    calcvar(T0);
    calcvar(S0);
    calcvar(P0);
    calcvar(G0);
    calcvar(G1);
    calcvar(G2);
    calcvar(Cv);
    calcvar(Vmin);
    calcvar(dPdV0);
    calcvar(dPdV1);
    calcvar(dedV0);
    calcvar(dedV1);
    calc.DynamicArray("Pref",128);
    calc.DynamicArray("eref",128);
}

void PeSplineParams::PrintParams(ostream &out) const
{
    FmtFlags old = out.setf(ios::right, ios::adjustfield);
    out << "\t"   << setw(10) << "V0 = " << V0
        << "; \t" << setw(10) << "e0 = " << e0 << "\n"
        << "\t"   << setw(10) << "P0 = " << P0
        << "; \t" << setw(10) << "T0 = " << T0
        << "; \t" << setw(10) << "S0 = " << S0 << "\n"
        << "\t"   << setw(10) << "G0 = " << G0
        << "; \t" << setw(10) << "G1 = " << G1
        << "; \t" << setw(10) << "G2 = " << G2 << "\n"
        << "\t"   << setw(10) << "Cv = " << Cv << "\n"
        << "\t"   << setw(10) << "Vmin = "  << Vmin  << "\n";
    if( !std::isnan(dPdV0) )
        out << "\t" << setw(10) << "dPdV0 = " << dPdV0 << "\n";
    if( !std::isnan(dPdV1) )
        out << "\t" << setw(10) << "dPdV1 = " << dPdV1 << "\n";
    if( !std::isnan(dedV0) )
        out << "\t" << setw(10) << "dedV0 = " << dedV0 << "\n";
    if( !std::isnan(dedV1) )
        out << "\t" << setw(10) << "dedV1 = " << dedV1 << "\n";
    if( n_data > 0 )
    {
        int i;
        out << "\t#n_data = " << n_data << "\n";
        for( i=0; i<n_data; i+=5 )
        {
            int j;
            int jmax = min(i+5,n_data);
            out << "\tPref(" << i+1 << ") = " << setw(10) << P_data[i];
            for( j=i+1; j<jmax; j++ )
            {
                out << ", " << setw(10) << P_data[j];
            }
            out << "\n";
            out << "\teref(" << i+1 << ") = " << setw(10) << e_data[i];
            for( j=i+1; j<jmax; j++ )
            {
                out << ", " << setw(10) << e_data[j];
            }
            out << "\n";
        }
        out << "\t#  K0=" << K0   << ",  FD0=" << FD0  << "\n"    
            << "\t#Vmax=" << Vmax << ", Pmin=" << Pmin << "\n";    
    }
    out.setf(old, ios::adjustfield);  
}

int PeSplineParams::ConvertParams(Convert &convert, EOSbase::Error *EOSerror)
{
    double s_V, s_e, s_T;
    if(  !finite(s_V = convert.factor("V"))
            || !finite(s_e = convert.factor("e"))
            || !finite(s_T = convert.factor("T")) )
    {
        EOSerror->Log("PeSplineParams::ConvertParams",__FILE__,__LINE__,NULL,
                "failed\n");
        return 1;
    }
    double s_P = s_e/s_V;
    V0 *= s_V;
    e0 *= s_e;
    P0 *= s_P;
    T0 *= s_T;
    S0 *= s_e/s_T;
    Cv *= s_e/s_T;
    Vmin  *= s_V;
    dPdV0 *= s_P/s_V;
    dPdV1 *= s_P/s_V;
    dedV0 *= s_P;
    dedV1 *= s_P;
    for( int i=0; i < n_data; i++ )
    {
        P_data[i] *= s_P;
        e_data[i] *= s_e;
    }
    return Reinit(EOSerror);
}

#define FUNC "PeSplineParams::ParamsOK",__FILE__,__LINE__,NULL
int PeSplineParams::ParamsOK(Calc &calc, EOSbase::Error *EOSerror)
{
    if( std::isnan(V0) || isnan(e0) || isnan(P0) || isnan(T0) || isnan(S0) )
    {
        EOSerror->Log(FUNC,"variable not set: "
                "V0=%lf, e0=%lf, P0=%lf, T0=%lf, S0=%lf\n",
                V0, e0, P0, T0, S0);
        return 1;
    }
    if( std::isnan(G0) || isnan(G1) || isnan(Cv) )
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
        EOSerror->Log(FUNC,"bad parameters: G0=%lf\n", G0);
        return 1;
    }
    if( Cv <= 0 )
    {
        EOSerror->Log(FUNC,"bad parameters: Cv=%lf\n", Cv);
        return 1;
    }
    int Pn = calc.FetchDynamicArray("Pref",P_data);
    int en = calc.FetchDynamicArray("eref",e_data);
    if( Pn != en )
    {
        EOSerror->Log(FUNC,"Pn != en: %d, %d\n", Pn, en);
        return 1;
    }
    n_data = Pn;
    return init(EOSerror);
}
