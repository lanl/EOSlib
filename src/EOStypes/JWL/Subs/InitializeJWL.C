#include <iostream>
#include <iomanip>
using namespace std;

#include "EOS.h"
#include "JWL.h"

#define FUNC "JWL::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int JWL::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    Edet = e0 = 0.0;
    V0 = A = R1 = B = R2 = C = omega = NaN;
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
    calc.Variable("T0",&Tcj);
    calc.Variable("S0",&Scj);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
    double P0 = 0.0;    // initial pressure if Edet <= 0
    calcvar(P0);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( isnan(V0) || isnan(A) || isnan(R1) || isnan(B) || isnan(R2)
                  || isnan(omega) || isnan(Edet) )
    {
        EOSerror->Log(FUNC,"parameter not set: V0=%lf, A=%lf, R1=%lf, "
              "B=%lf, R2=%lf, omega=%lf, Edet=%lf\n",
               V0, A,R1, B,R2, omega, Edet);
        return 1;
    }
    de = Edet - e0;
    if( Edet <= 0.0 )
    { // replace (Vcj,ecj) with isentrope thru P0 = P(V0,eref0)
        C  = P0-A*exp(-R1)-B*exp(-R2);
        Vcj = V0;
        if( isnan(e_ref) )
            e_ref = V0*((A/R1)*exp(-R1)+(B/R2)*exp(-R2)+C/omega) -de;       
        if( isnan(Tcj) )
            Tcj = (V0/omega)*(P0-A*exp(-R1)-B*exp(-R2))/Cv;
        if( isnan(Scj) )
            Scj = 0.;
    }
    else if( de > 0. )
    {
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
        delete det;
        if( isnan(C) )
            C = omega*pow(Vcj/V0,omega)
                *((CJ.e+de)/V0-(A/R1)*exp(-R1*Vcj/V0)-(B/R2)*exp(-R2*Vcj/V0));
        if( isnan(Tcj) )
            Tcj = (CJ.e+de -V0*(A/R1*exp(-R1*Vcj/V0)+B/R2*exp(-R2*Vcj/V0)))/Cv;
        if( isnan(Scj) )
        {
            double T0 = T(V0,e0);
            Scj = Edet/T0 - Cv*(log(T0/Tcj)+omega*log(V0/Vcj));
        }
    }
    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(e_ref) )
        e_ref = e0;
    return 0;
}


void JWL::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t" << setw(10) << "V0    = " << V0 << "\n"
	    << "\t" << setw(10) << "e0    = " << e0 << "\n";
    if( Edet <= 0.0 )
    {
        double P0 = A*exp(-R1) + B*exp(-R2) + C;
        out << "\t" << setw(10) << "P0    = " << P0 << "\n";
    }
	out << "\t"   << setw(10) << "A     = " << A
	    << "; \t" << setw(10) << "R1    = " << R1 << "\n"
	    << "\t"   << setw(10) << "B     = " << B
	    << "; \t" << setw(10) << "R2    = " << R2 << "\n"
	    << "\t"   << setw(10) << "omega = " << omega << "\n"
        << "\t"   << setw(10) << "Edet    = " << Edet << "\n";
    if( !isnan(C) )
        out << "\t"   << setw(10) << "C     = " << C << "\n";
    if( !isnan(Tcj) )
    {
        out << "\t"   << setw(10) << "Cv      = " << Cv  << "\n";
        if( !isnan(Pcj) )
        {
            out << "\t"   << setw(10) << "Tcj     = " << Tcj
                << "; \t" << setw(10) << "Scj     = " << Scj << "\n";
            double Dcj = V0*sqrt(Pcj/(V0-Vcj));
            double ucj = sqrt(Pcj*(V0-Vcj));
            out << "\t#"   << setw(10) << "Vcj     = " << Vcj
	            << "; \t"  << setw(10) << "Pcj     = " << Pcj << "\n"
                << "\t#"  << setw(10) << "Dcj     = " << Dcj
	            << "; \t" << setw(10) << "ucj     = " << ucj
	            << "; \t" << setw(10) << "ccj     = " << Dcj-ucj << "\n";
        }
        else
        {
            out << "\t"   << setw(10) << "T0      = " << Tcj
                << "; \t" << setw(10) << "S0      = " << Scj << "\n";
        }
    }
	out.setf(old, ios::adjustfield);	
}

#undef  FUNC
#define FUNC "JWL::ConvertParams",__FILE__,__LINE__,this
int JWL::ConvertParams(Convert &convert)
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
    A  *= s_P;
    B  *= s_P;
    C  *= s_P;
    Edet *= s_e;
    de *= s_e;
    // R1, R2, omega are dimensionless
	Cv  *= s_e/s_T;    
    Scj *= s_e/s_T;
    Tcj *= s_T;
    Vcj *= s_V;
    Pcj *= s_P;
    return 0;
}
