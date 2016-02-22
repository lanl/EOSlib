#include "StiffIdeal_VT.h"
#include "StiffIdeal.h"

StiffIdeal_VT::StiffIdeal_VT() : EOS_VT("StiffIdeal")
{
    lambda1 = lambda2 = -1.;
    G1 = Cv1 = NaN;
    G2 = Cv2 = NaN;
    P_p = Q = dS = 0.;
    P_ref = e_ref = NaN;        
    V1 = V2 = 0.0;
    e1 = e2 = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = NaN;
}

StiffIdeal_VT::StiffIdeal_VT(StiffIdeal &eos) : EOS_VT("StiffIdeal")
{
    Transfer(eos);
    P_ref = eos.Peq;
    e_ref = eos.e_ref;      
    lambda1 = eos.lambda1;
    lambda2 = eos.lambda2;
    G1  = eos.G1;
    P_p = eos.P_p;
    Cv1 = eos.Cv1;
    G2  = eos.G2;
    Q   = eos.Q;
    Cv2 = eos.Cv2;
    dS  = eos.dS;
    //
    V1 = V2 = 0.0;
    e1 = e2 = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = NaN;
}

StiffIdeal_VT::~StiffIdeal_VT()
{
	// Null   
}

EOS *StiffIdeal_VT::Ve()
{
    return new StiffIdeal(*this);
}

void StiffIdeal_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    out << "\tlambda1 = " << lambda1 << "\n";
    out << "\tG1      = " << G1      << "\n";
    out << "\tP_p     = " << P_p     << "\n";
    out << "\tCv1     = " << Cv1     << "\n";
    out << "\tG2      = " << G2      << "\n";
    out << "\tQ       = " << Q       << "\n";
    out << "\tCv2     = " << Cv2     << "\n";
    out << "\tdS      = " << dS      << "\n";    
}

int StiffIdeal_VT::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
	double s_V, s_e, s_C;
	if( !finite(s_C = convert.factor("Cv"))
	     || !finite(s_V = convert.factor("V"))
	         || !finite(s_e = convert.factor("e"))	)
	{
	    return 1;
	}
	P_ref *= s_e/s_V;
	e_ref *= s_e;
	P_p *= s_e/s_V;
	Cv1 *= s_C;
	Q   *= s_e;
	Cv2 *= s_C;
    dS  *= s_C;
    V1 = V2 = 0.0;
    e1 = e2 = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = NaN;
    return 0;    
}

#define FUNC "StiffIdeal_VT::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int StiffIdeal_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    V1 = V2 = 0.0;
    e1 = e2 = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = NaN;
    //    
    lambda1 = 1.;   // default: all Stiffened gas
    G1 = Cv1 = NaN;
    G2 = Cv2 = NaN;
    P_p = Q = 0.;   // default: two ideal gases
    dS = NaN;
    calcvar(lambda1);
    calcvar(G1);    // G1 = gamma_1 -1
    calcvar(P_p);
    calcvar(Cv1);
    calcvar(G2);    // G2 = gamma_2 -1
    calcvar(Q);
    calcvar(Cv2);
    calcvar(dS);
    double c0 = -1.;
    double s = 0.;
    // Stiffened gas match to us = c0 +s*up => G1 && P_p
    calcvar(c0);
    calcvar(s);
    P_ref = e_ref = NaN;        
    
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( lambda1 < 0 || 1 < lambda1 )
    {
        EOSerror->Log(FUNC, "lambda not between 0 and 1\n" );
        return 1;
    }
    lambda2 = 1. - lambda1;
    if( isnan(V_ref) || isnan(T_ref) )
    {
        EOSerror->Log(FUNC, "V_ref and T_ref not specified\n" );
        return 1;            
    }
    if( Pequil(V_ref,T_ref) )
    {
        EOSerror->Log(FUNC, "Bad ref state\n" );
        return 1;            
    }
    P_ref = Peq;
    e_ref = eav;  
    if( c0 > 0. && s > 1. )
    {
        G1  = 4.*s-2.;
        P_p = c0*c0/V_ref/(G1+1.) - P_ref;
    }   
    if( isnan(G1) || G1 < 0. )
    {
        EOSerror->Log(FUNC, "G1 not defined or invalid\n" );
        return 1;        
    }
    if( isnan(G2) || G2 < 0. )
    {
        EOSerror->Log(FUNC, "G2 not defined or invalid\n" );
        return 1;        
    }
    if( isnan(Cv1) || Cv1 <= 0. )
    {
        EOSerror->Log(FUNC, "Cv1 not defined or invalid\n" );
        return 1;        
    }
    if( isnan(Cv2) || Cv2 <= 0. )
    {
        EOSerror->Log(FUNC, "Cv2 not defined or invalid\n" );
        return 1;        
    }
    if( isnan(dS) )
    {
        if( Q > 0. )
        {
            if( isnan(T_ref) )
            {
                EOSerror->Log(FUNC, "T_ref needed to set dS\n" );
                return 1;            
            }
            dS = Q/T_ref;
        }
        else
            dS = 0.;
    }
    return 0;
}
