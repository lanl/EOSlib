#include <iostream>
#include <iomanip>
using namespace std;

#include "EOS.h"
#include "HEprods.h"


#define FUNC "HEprods::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int HEprods::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    V0= 0.0;
    e0= 0.0;
    P0= 0.0;
    calcvar(V0);
    calcvar(e0);
    calcvar(P0);

    rho_min = 0.0;
    rho_max = 0.0;
    calcvar(rho_min);
    calcvar(rho_max);

    rho_sw = 0.0;
    calcvar(rho_sw);
    gamma1 = 0.0;
    gamma2 = 0.0;
    calcvar(gamma1);
    calcvar(gamma2);
    na1 = 0;
    na2 = 0;
    calcvar(na1);
    calcvar(na2);
    calc.Array("a1",a1,12);
    calc.Array("a2",a2,12);

    nb1 = 0;
    nb2 = 0;
    calcvar(nb1);
    calcvar(nb2);
    calc.Array("b1",b1,12);
    calc.Array("b2",b2,12);

    Cv = 0.0;
    calcvar(Cv);
    T_cj = 0.0;
    S_cj = 0.0;
    calcvar(T_cj);
    calcvar(S_cj);

    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }

    // Parameter check
    if( V0 <= 0.0 )
    {
        EOSerror->Log(FUNC,"V0<=0\n");
        return 1;
    }
    if( P0 < 0.0 )
    {
        EOSerror->Log(FUNC,"P0=0\n");
        return 1;
    }
    if( T_cj <= 0.0 )
    {
        EOSerror->Log(FUNC,"T_cj<=0\n");
        return 1;
    }
    if( S_cj <= 0.0 )
    {
        EOSerror->Log(FUNC,"S_cj<=0\n");
        return 1;
    }

    if( rho_min<=0.0 || rho_max<=rho_min )
    {
        EOSerror->Log(FUNC,"improper bounds: rho_min=%lf, rho_max=%lf\n",
                      rho_min, rho_max);
        return 1;
    }
    if( rho_sw<rho_min || rho_sw>rho_max )
    {
        EOSerror->Log(FUNC,"rho_sw not between rho_min and rho_max\n");
        return 1;
    }

    if( gamma1<=1. || gamma2<=1. )
    {
        EOSerror->Log(FUNC,"gamma1 or gamma2 <= 1\n");
        return 1;
    }

    if( na1<=0 || na2<=0 )
    {
        EOSerror->Log(FUNC,"na1 or na2 <= 0\n");
        return 1;
    }
    if( na1>12 || na2>12 )
    {
        EOSerror->Log(FUNC,"na1 or na2 > max (12)\n");
        return 1;
    }
    for( int i=0; i<na1; i++)
    {
        if( std::isnan(a1[i]) )
        {
            EOSerror->Log(FUNC,"a1[%d] not defined\n",i);
            return 1;
        }
    }
    for( int i=0; i<na2; i++)
    {
        if( std::isnan(a2[i]) )
        {
            EOSerror->Log(FUNC,"a2[%d] not defined\n",i);
            return 1;
        }
    }

    if( nb1<=0 || nb2<=0 )
    {
        EOSerror->Log(FUNC,"nb1 or nb2 <= 0\n");
        return 1;
    }
    if( nb1>12 || nb2>12 )
    {
        EOSerror->Log(FUNC,"nb1 or nb2 > max (12)\n");
        return 1;
    }
    for( int i=0; i<nb1; i++)
    {
        if( std::isnan(a1[i]) )
        {
            EOSerror->Log(FUNC,"a1[%d] not defined\n",i);
            return 1;
        }
    }
    for( int i=0; i<nb2; i++)
    {
        if( std::isnan(a2[i]) )
        {
            EOSerror->Log(FUNC,"a2[%d] not defined\n",i);
            return 1;
        }
    }

    if( Cv <= 0.0 )
    {
        EOSerror->Log(FUNC,"Cv<=0\n");
        return 1;
    }

    // CJ state
    rho_cj = FindCJ();
    if( std::isnan(rho_cj) )
    {
        EOSerror->Log(FUNC,"FindCJ failed\n");
        return 1;
    }
    double Vcj = 1./rho_cj;
    P_cj = Pref(Vcj);
    e_cj = 0.5*(P_cj+P0)*(V0-Vcj)+e0;
    e_sw = eref(1./rho_sw);
    T_sw = Tref(1./rho_sw);

    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = e0;

    return 0;
}


void HEprods::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);

	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 
	    << "; \t" << setw(10) << "P0 = " << P0 << "\n"
        
	    << "\t"   << setw(10) << "rho_cj = " << rho_cj
	    << "; \t" << setw(10) << "  e_cj = " << e_cj 
	    << "; \t" << setw(10) << "  P_cj = " << P_cj << "\n"

	    << "\t"   << setw(10) << "rho_min = " << rho_min
	    << "; \t" << setw(10) << "rho_max = " << rho_max << "\n"

        
	    << "\t"   << setw(10) << "rho_sw = " << rho_sw
	    << "\t"   << setw(10) << "gamma1 = " << gamma1
	    << "; \t" << setw(10) << "gamma2 = " << gamma2 << "\n"

        << "\t"   << setw(10) << "na1 = " << na1;
    FmtFlags old1 = out.setf(ios::scientific, ios::floatfield);
    int prec = out.precision(12);
    for(int i=0; i<na1; i++)
    {
        if( i%2 == 0 )
          cout << "\n\t" << setw(10) << "a1(" << i << ") = " << a1[i];          
        else
          out << ", " << a1[i];
    }
	out << "\n\t" << setw(10) << "na2 = " << na2;
    for(int i=0; i<na2; i++)
    {
        if( i%2 == 0 )
          cout << "\n\t" << setw(10) << "a2(" << i << ") = " << a2[i];          
        else
          out << ", " << a2[i];
    }
	out << "\n\t" << setw(10) << "nb1 = " << nb1;
    for(int i=0; i<nb1; i++)
    {
        if( i%2 == 0 )
          cout << "\n\t" << setw(10) << "b1(" << i << ") = " << b1[i];          
        else
          out << ", " << b1[i];
    }
	out << "\n\t" << setw(10) << "nb2 = " << nb2;
    for(int i=0; i<nb2; i++)
    {
        if( i%2 == 0 )
          cout << "\n\t" << setw(10) << "b2(" << i << ") = " << b2[i];          
        else
          out << ", " << b2[i];
    }
    out.precision(prec);
    out.setf(old1, ios::floatfield);

    out << "\n\t" << setw(10) << "Cv = "   << Cv << "\n"
        << "\t"   << setw(10) << "T_cj = " << T_cj
        << "; \t" << setw(10) << "S_cj = " << S_cj << "\n";

	out.setf(old, ios::adjustfield);	
}

#undef  FUNC
#define FUNC "HEprods::ConvertParams",__FILE__,__LINE__,this
int HEprods::ConvertParams(Convert &convert)
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

    rho_min /= s_V;
    rho_max /= s_V;
    rho_sw  /= s_V;
      e_sw  *= s_e;
      T_sw  *= s_T;

    Cv   *= s_e/s_T;
    T_cj *= s_T;
    S_cj *= s_e/s_T;

    rho_cj /= s_V;
      e_cj *= s_e;
      P_cj *= s_P;

    double s = s_P*pow(s_V,gamma1);
    for( int i=0; i<na1; ++i )
    {
        a1[i] *= s;
        s *= s_V;
    }
    s = s_P*pow(s_V,gamma2);
    for( int i=0; i<na2; ++i )
    {
        a2[i] *= s_P;
        s *= s_V;
    }

    s = 1./s_V;
    for( int i=0; i<nb1; ++i )
    {
        b1[i] *= s;
        s *= s_V;
    }
    s = 1./s_V;
    for( int i=0; i<nb2; ++i )
    {
        b2[i] *= s;
        s *= s_V;
    }

    return 0;
}
