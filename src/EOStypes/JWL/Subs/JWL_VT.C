#include "JWL_VT.h"
#include "JWL.h"

#include <iostream>
#include <iomanip>
using namespace std;

JWL_VT::JWL_VT() : EOS_VT("JWL")
{
    e0 = 0.0;
    V0 = NaN;
    A  = R1 = NaN;
    B  = R2 = NaN;
    C  = omega = NaN;
    de = Edet = NaN;
    Cv = Scj = Tcj = Vcj = Pcj = NaN;
}


JWL_VT::JWL_VT(JWL &eos) : EOS_VT("JWL")
{
    Transfer(eos);
    V0    = eos.V0;
    e0    = eos.e0;
    A     = eos.A;
    R1    = eos.R1;
    B     = eos.B;
    R2    = eos.R2;
    C     = eos.C;
    omega = eos.omega;
    Edet  = eos.Edet;
    de    = eos.de;
    Cv    = eos.Cv;
    Tcj   = eos.Tcj;
    Scj   = eos.Scj;
    Vcj   = eos.Vcj;
    Pcj   = eos.Pcj;
    EOSstatus = eos.status();
}

JWL_VT::~JWL_VT()
{
	// Null
}

EOS *JWL_VT::Ve()
{
    return new JWL(*this);
}

void JWL_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0    = " << V0
	    << "; \t" << setw(10) << "e0    = " << e0 << "\n"
	    << "\t"   << setw(10) << "A     = " << A
	    << "; \t" << setw(10) << "R1    = " << R1 << "\n"
	    << "\t"   << setw(10) << "B     = " << B
	    << "; \t" << setw(10) << "R2    = " << R2 << "\n"
	    << "\t"   << setw(10) << "C     = " << C  << "\n"
	    << "\t"   << setw(10) << "omega = " << omega << "\n"
        << "\t"   << setw(10) << "Edet    = " << Edet << "\n";
    if( !std::isnan(Tcj) )
    {
        double Dcj = V0*sqrt(Pcj/(V0-Vcj));
        double ucj = sqrt(Pcj*(V0-Vcj));
        out << "\t"   << setw(10) << "Cv      = " << Cv << "\n"
            << "\t"   << setw(10) << "Tcj     = " << Tcj
            << "; \t" << setw(10) << "Scj     = " << Scj << "\n"
            << "\t#"  << setw(10) << "Vcj     = " << Vcj
	        << "; \t" << setw(10) << "Pcj     = " << Pcj << "\n"
            << "\t#"  << setw(10) << "Dcj     = " << Dcj
	        << "; \t" << setw(10) << "ucj     = " << ucj
	        << "; \t" << setw(10) << "ccj     = " << Dcj-ucj << "\n";
    }
	out.setf(old, ios::adjustfield);	
}

#undef  FUNC
#define FUNC "JWL_VT::ConvertParams",__FILE__,__LINE__,this
int JWL_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
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
    A  *= s_P;
    B  *= s_P;
    C  *= s_P;
    Edet *= s_e;
    de   *= s_e;
    // R1, R2, omega are dimensionless
	Cv  *= s_e/s_T;    
	Scj *= s_e/s_T;    
    Tcj *= s_T;
    Vcj *= s_V;
    Pcj *= s_P;
    return 0;
}

#undef  FUNC
#define FUNC "JWL_VT::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int JWL_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    e0 = 0.0;
    V0 = A = R1 = B = R2 = C = omega = Edet = NaN;
    Cv = Scj = Tcj = Pcj = Vcj = NaN;
    calcvar(V0);
    calcvar(e0);
    calcvar(A);
    calcvar(R1);
    calcvar(B);
    calcvar(R2);
    calcvar(C);
    calcvar(omega);
    calcvar(Edet);
    calcvar(Cv);
    calcvar(Tcj);
    calcvar(Scj);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);

    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( std::isnan(V0) || std::isnan(A) || std::isnan(R1) || std::isnan(B) || std::isnan(R2)
        || std::isnan(omega) || std::isnan(Edet) )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, A=%lf, R1=%lf, "
              "B=%lf, R2=%lf, omega=%lf, Edet=%lf\n",
               V0, A,R1, B,R2, omega, Edet);
        return 1;
    }
    de = Edet - e0;
    if( !std::isnan(Tcj) )
    {
        JWL HE(*this);
        Vcj = HE.Vcj;
        Pcj = HE.Pcj;
        C = HE.Pcj;
        Tcj = HE.Tcj;
        Scj = HE.Scj;
    }
    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(T_ref) )
        T_ref = Tref(V_ref) + (e0-eref(V_ref))/Cv;
    return 0;
}
