#include "StiffIdeal.h"
#include "StiffIdeal_VT.h"

StiffIdeal::StiffIdeal() : EOS("StiffIdeal")
{
    lambda1 = lambda2 = -1.;
    G1 = Cv1 = NaN;
    G2 = Cv2 = NaN;
    P_p = Q = dS = 0.;
    V1 = V2 = 0.0;
    e1 = e2 = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = NaN;
}

StiffIdeal::StiffIdeal(const StiffIdeal &eos) : EOS(eos)
{
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

StiffIdeal::StiffIdeal(StiffIdeal_VT &eos) : EOS("StiffIdeal")
{
    Transfer(eos);
    V_ref = eos.V_ref;
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

StiffIdeal::~StiffIdeal()
{
	// Null   
}

EOS_VT *StiffIdeal::VT()
{
    return new StiffIdeal_VT(*this);
}

void StiffIdeal::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    out << "\tlambda1 = " << lambda1 << "\n";
    out << "\tG1      = " << G1      << "\n";
    out << "\tP_p     = " << P_p     << "\n";
    out << "\tCv1     = " << Cv1     << "\n";
    out << "\tG2      = " << G2      << "\n";
    out << "\tQ       = " << Q       << "\n";
    out << "\tCv2     = " << Cv2     << "\n";
    out << "\tdS      = " << dS      << "\n";
}

int StiffIdeal::ConvertParams(Convert &convert)
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

#define FUNC "StiffIdeal::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int StiffIdeal::InitParams(Parameters &p, Calc &calc, DataBase *db)
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
    
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    double P_ref, T_ref;
    calc.Fetch("P_ref",P_ref); // added to calc by EOS::InitBase
    calc.Fetch("T_ref",T_ref); // added to calc by EOS::InitBase
    if( lambda1 < 0 || 1 < lambda1 )
    {
        EOSerror->Log(FUNC, "lambda not between 0 and 1\n" );
        return 1;
    }
    lambda2 = 1. - lambda1;
    if( c0 > 0. && s > 1. )
    {
        if( std::isnan(V_ref) || std::isnan(P_ref) )
        {
            EOSerror->Log(FUNC, "V_ref and P_ref needed to use c0 and s\n" );
            return 1;            
        }
        G1  = 4.*s-2.;
        P_p = c0*c0/V_ref/(G1+1.) - P_ref;
        // note : Assumes lambda1 = 1
        //        EOS::InitBase sets T_ref = 300
        //        then computes V_ref and e_ref from P_ref and T_ref
        e_ref = (P_ref+(G1+1.)*P_p)*V_ref/G1;
        T_ref = (e_ref-P_p*V_ref)/Cv1;
        CalcVar *Tcalc =calc.Fetch("T_ref");
        Tcalc->set(T_ref);
    }   
    if( std::isnan(G1) || G1 < 0. )
    {
        EOSerror->Log(FUNC, "G1 not defined or invalid\n" );
        return 1;        
    }
    if( std::isnan(G2) || G2 < 0. )
    {
        EOSerror->Log(FUNC, "G2 not defined or invalid\n" );
        return 1;        
    }
    if( std::isnan(Cv1) || Cv1 <= 0. )
    {
        EOSerror->Log(FUNC, "Cv1 not defined or invalid\n" );
        return 1;        
    }
    if( std::isnan(Cv2) || Cv2 <= 0. )
    {
        EOSerror->Log(FUNC, "Cv2 not defined or invalid\n" );
        return 1;        
    }
    if( std::isnan(dS) )
    {
        if( Q > 0. )
        {
            if( std::isnan(T_ref) )
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
