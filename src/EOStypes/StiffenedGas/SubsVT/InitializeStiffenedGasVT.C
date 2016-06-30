#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

#define FUNC "StiffenedGas_VT::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int StiffenedGas_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    T_ref = 300;
    StiffGasParams::InitParams(calc);
    // alternative parameters specifying model
	double P0 = NaN;    // pressure at ref state
	double c0 = NaN;    // sound speed at ref state
	double s  = NaN;    // du_s/du_p on principal Hugoniot
	calcvar(P0);
	calcvar(c0);
	calcvar(s);

	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    	
    if( !std::isnan(s) )
    { // s = 0.5*FundDeriv = 0.5*(1+0.5*G)
        G = 2*(2*s-1);
    }
    else if( !finite(G) )
    {
        EOSerror->Log(FUNC,"G not set\n");
        return 1;
    }
    
	if( !std::isnan(c0) )
	{ // assume V_0 = V_ref and e_0 = 0
	    if( std::isnan(P0) || isnan(V_ref)  )
	    {
	        EOSerror->Log(FUNC,"must specify P0 and V_ref with c0\n");
	    	return 1;
	    }
	    P_p = c0*c0/V_ref/(G+1) - P0;
	    e_p = P0 *V_ref - c0*c0/G;
        T_ref = -(e_p + P_p*V_ref)/Cv;
	}
    
    if( !finite(e_p) || !finite(P_p) || !finite(Cv) )
    {
        EOSerror->Log(FUNC,"variable not set: e_p=%lf, P_p=%lf, Cv=%lf\n", e_p, P_p, Cv);
        return 1;
    }
    
	return 0;
}

