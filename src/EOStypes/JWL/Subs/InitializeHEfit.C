#include <iostream>
#include <iomanip>
using namespace std;

#include "EOS.h"
#include "HEfit.h"

#define FUNC "HEfit::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int HEfit::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0 = NaN;
    P0 = e0 = 0;
    rho_CJ = P_CJ = e_CJ = NaN;
    rho_min = rho_max = NaN;
    Pmin = Pmax = NaN;
    Gamma0 = NaN;
      
    nfCJ = -1;
    ngCJ = -1;
    for(int i=0; i<10; i++)
    {
        fCJ[i] = NaN;
        gCJ[i] = NaN;
    }
    
    calcvar(V0);
    calcvar(e0);
    calcvar(P0);
    calcvar(rho_CJ);
    calcvar(e_CJ);
    calcvar(P_CJ);
    calcvar(rho_min);
    calcvar(rho_max);
    calcvar(Pmin);
    calcvar(Pmax);
    calcvar(Gamma0);
    calcvar(nfCJ);
    calcvar(ngCJ);
    calc.Array("fCJ",fCJ,10);
    calc.Array("gCJ",gCJ,10);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);

    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }

    if( std::isnan(V0) || std::isnan(rho_CJ) || std::isnan(P_CJ)
        || std::isnan(rho_min)|| std::isnan(rho_max) )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, rho_CJ=%lf, P_CJ=%lf\n"
              "\trho_min=%lf, rho_max=%lf, Pmin=%lf, Pmax=%lf, Gamma0=%lf\n",
               V0, rho_CJ,P_CJ, rho_min,rho_max, Pmin,Pmax, Gamma0);
        return 1;
    }
    e_CJ = e0 + 0.5*(P_CJ+P0)*(V0 -1/rho_CJ);

    int i;
    if( 0 <= nfCJ && nfCJ < 10 )
    {
        for( i =0; i<nfCJ; i++)
        {
            if( std::isnan(fCJ[i]) )
            {
                nfCJ = -1;
                break;
            }
        }
    }
    if( nfCJ < 0 )
    {
        EOSerror->Log(FUNC,"nfCJ or fCJ not set\n");
        return 1;
    }

    if( 0 <= ngCJ && ngCJ < 10 )
    {
        for( i =0; i<ngCJ; i++)
        {
            if( std::isnan(gCJ[i]) )
            {
                ngCJ = -1;
                break;
            }
        }
    }
    if( ngCJ < 0 )
    {
        EOSerror->Log(FUNC,"ngCJ or gCJ not set\n");
        return 1;
    }

    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = e0;
   
    double y[1];
    y[0] = e_CJ;
    double drho = 0.001*P_CJ/dPdrho(rho_CJ);
    int status = ODE::Initialize(rho_CJ, y, drho);
    if( status )
    {
        EOSerror->Log(FUNC,"ODE.Initialize failed with status: %s\n",
              ODE::ErrorStatus(status) );
        nfCJ = -1;
        return 1;
    }
    
    return 0;
}


void HEfit::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 
	    << "; \t" << setw(10) << "P0 = " << P0 << "\n"
        
	    << "\t"   << setw(10) << "rho_CJ = " << rho_CJ
	    << "; \t" << setw(10) << "  e_CJ = " << e_CJ 
	    << "; \t" << setw(10) << "  P_CJ = " << P_CJ << "\n"

	    << "\t"   << setw(10) << "rho_min = " << rho_min
	    << "; \t" << setw(10) << "rho_max = " << rho_max << "\n"

	    << "\t"   << setw(10) << "Pmin = " << Pmin
	    << "; \t" << setw(10) << "Pmax = " << Pmax << "\n"
        
	    << "\t"   << setw(10) << "Gamma0 = " << Gamma0 << "\n"

        << "\t"   << setw(10) << "nfCJ = " << nfCJ << "\n"
        << "\t"   << setw(10) << "fCJ(1) = " << fCJ[0];
    int i;
    for(i=1; i<=nfCJ; i++)
        out << ", " << fCJ[i];
    out << "\n\t" << setw(10) << "ngCJ = " << ngCJ << "\n"
        << "\t"   << setw(10) << "gCJ(1) = " << gCJ[0];
    for(i=1; i<=ngCJ; i++)
        out << ", " << gCJ[i];
	out << "\n";
	out.setf(old, ios::adjustfield);	
}

#undef  FUNC
#define FUNC "HEfit::ConvertParams",__FILE__,__LINE__,this
int HEfit::ConvertParams(Convert &convert)
{
/*********************
 * should redefine fCJ for series in rho/rho_CJ rather than rho
 *        same for gCJ
*********************/
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	double s_V, s_e;
	if( !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e")) )
	{
	    EOSerror->Log(FUNC, "failed\n");
	    return 1;
	}
    double s_P = s_e/s_V;
    V0 *= s_V;
    e0 *= s_e;
    P0 *= s_P;
    rho_CJ /= s_V;
    e_CJ *= s_e;
    P_CJ *= s_P;
    rho_min /= s_V;
    rho_max /= s_V;
    Pmin *= s_P;
    Pmax *= s_P;
    // Gamma0 dimensionless
    int i;
    double s_f = s_P*pow(s_V, Gamma0);
    for(i=0; i<=nfCJ; i++)
    {
        fCJ[i] *=s_f;
        s_f *= s_V;
    }
    double s_g = s_P/s_e;
    for(i=0; i<=ngCJ; i++)
    {
        gCJ[i] *=s_g;
        s_g *= s_V;
    }
    double y[1];
    y[0] = e_CJ;
    double drho = 0.001*P_CJ/dPdrho(rho_CJ);
    status = ODE::Initialize(rho_CJ, y, drho);
    if( status )
    {
	    EOSerror->Log(FUNC,"ODE.Initialize failed with status: %s\n",
                       ODE::ErrorStatus(status) );
        nfCJ = -1;
        return 1;
    }

    return 0;
}
