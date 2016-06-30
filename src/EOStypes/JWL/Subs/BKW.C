#include "BKW.h"
#include "BKW_VT.h"

#include <iostream>
#include <iomanip>
using namespace std;

BKW::BKW() : EOS("BKW")
{
    V0 = NaN;
    e0 = 0.0;
    P0 = 0.0;
    de = 0.0;
    Cv = NaN;
    Scj = NaN;
    Vcj = Pcj = Tcj = NaN;
    Punit = eunit = Vunit = Tunit = 1.0;
    int i;
    for( i=0; i<5; i++ )
        Pn[i] = en[i] = Tn[i] = 0.0;
}

BKW::~BKW()
{
	// Null
}

BKW::BKW(BKW_VT &eos) : EOS("BKW")
{
    Transfer(eos);
    V0 = eos.V0;
    e0 = eos.e0;
    P0 = eos.P0;
    de = eos.de;
    Cv = eos.Cv;
    Scj = eos.Scj;
    Vcj = eos.Vcj;
    Pcj = eos.Pcj;
    Tcj = eos.Tcj;
    int i;
    for( i=0; i<5; i++ )
    {
        Pn[i] = eos.Pn[i];
        en[i] = eos.en[i];
        Tn[i] = eos.Tn[i];
    }
    Punit = eos.Punit;
    eunit = eos.eunit;
    Vunit = eos.Vunit;
    Tunit = eos.Tunit;
    EOSstatus = eos.status();
}

EOS_VT *BKW::VT()
{
    return new BKW_VT(*this);
}

#undef  FUNC
#define FUNC "BKW::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int BKW::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0 = NaN;
    e0 = 0.0;
    P0 = 0.0;
    de = 0.0;
    Cv = NaN;
    Scj = NaN;
    Vcj = Pcj = Tcj = NaN;
    Punit = eunit = Vunit = Tunit = 1.0;
    int i;
    for( i=0; i<5; i++ )
        Pn[i] = en[i] = Tn[i] = 0.0;
    calcvar(V0);
    calcvar(e0);
    calcvar(P0);
    calcvar(de);
    calcvar(Cv);
    calcvar(Scj);
    calcvar(Punit);
    calcvar(eunit);
    calcvar(Vunit);
    calcvar(Tunit);
    calc.Array("Pn",Pn,5);
    calc.Array("en",en,5);
    calc.Array("Tn",Tn,5);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( std::isnan(V0) || isnan(Cv) )
    {
        EOSerror->Log(FUNC,"parameter not all set\n");
        return 1;
    }
    double p0 = P(V0,e0);
    double T0 = T(V0,e0);
    if( std::isnan(p0) || isnan(T0) )
    {
        EOSerror->Log(FUNC,"P or T failed at (V0,e0)\n");
        return 1;
    }    
    // CJ state
    HydroState state0;
    state0.V = V0;
    state0.e = e0;
    WaveState CJ;
    Detonation *det = detonation(state0,P0);
    if( det == NULL || det->CJwave(RIGHT,CJ) )
    {
        EOSerror->Log(FUNC,"failed to find CJ state\n");
        return 1;
    }
    Vcj = CJ.V;
    Pcj = CJ.P;
    Tcj = T(Vcj,CJ.e);
    if( std::isnan(Scj) )
    {   // pick something reasonable ?
        Scj = Vcj*Pcj/Tcj;
        // If one knew reactants EOS
        // then Tcj*(Scj-Sreactants) = Hcj - Hreactants(Pcj,Tcj)
        // where H = e + PV = enthalpy
    }
    delete det;
    //
    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = e0;
    return 0;
}

void BKW::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t" << setw(10) << "V0    = " << V0  << "\n"
	    << "\t" << setw(10) << "e0    = " << e0  << "\n"
	    << "\t" << setw(10) << "P0    = " << P0  << "\n"
	    << "\t" << setw(10) << "de    = " << de  << "\n"
	    << "\t" << setw(10) << "Cv    = " << Cv  << "\n"
	    << "\t" << setw(10) << "Scj   = " << Scj << "\n";
    out << "\t" << setw(10) << "Pn(1) = " << Pn[0] << ", "
                                          << Pn[1] <<", "
                                          << Pn[2] <<", "
                                          << Pn[3] <<", "
                                          << Pn[4] <<"\n";
    out << "\t" << setw(10) << "en(1) = " << en[0] << ", "
                                          << en[1] <<", "
                                          << en[2] <<", "
                                          << en[3] <<", "
                                          << en[4] <<"\n";
    out << "\t" << setw(10) << "Tn(1) = " << Tn[0] << ", "
                                          << Tn[1] <<", "
                                          << Tn[2] <<", "
                                          << Tn[3] <<", "
                                          << Tn[4] <<"\n";
	out << "\t" << setw(10) << "Punit = " << Punit << "\n"
	    << "\t" << setw(10) << "eunit = " << eunit << "\n"
	    << "\t" << setw(10) << "Vunit = " << Vunit << "\n"
	    << "\t" << setw(10) << "Tunit = " << Tunit << "\n";
    double Dcj = V0*sqrt(Pcj/(V0-Vcj));
    double ucj = sqrt(Pcj*(V0-Vcj));
    out << "\t#"   << setw(10) << "Vcj = " << Vcj
	    << "; \t"  << setw(10) << "Pcj = " << Pcj
	    << "; \t"  << setw(10) << "Tcj = " << Tcj << "\n"
        << "\t#"   << setw(10) << "Dcj = " << Dcj
	    << "; \t"  << setw(10) << "ucj = " << ucj
	    << "; \t"  << setw(10) << "ccj = " << Dcj-ucj << "\n";
	out.setf(old, ios::adjustfield);	
}

#undef  FUNC
#define FUNC "BKW::ConvertParams",__FILE__,__LINE__,this
int BKW::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	double s_V, s_e, s_T;
	if( !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	     || !finite(s_T = convert.factor("T")) )
	{
	    EOSerror->Log(FUNC, "failed\n");
	    return 1;
	}
    double s_P = s_e/s_V;
    V0 *= s_V;
    e0 *= s_e;
    P0 *= s_P;
    de *= s_e;
	Cv  *= s_e/s_T;    
    Scj *= s_e/s_T;
    Vcj *= s_V;
    Pcj *= s_P;
    Tcj *= s_T;
    Punit *= s_P;
    eunit *= s_e;
    Vunit *= s_V;
    Tunit *= s_T;    
    return 0;
}
