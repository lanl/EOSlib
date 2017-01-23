#include <OneDFunction.h>

#include "EOS.h"
#include "Isotherm_ODE.h"



class e_EOS : private OneDFunction
{
    friend class EOS;
private:
    EOS *eos;
    double V;

    e_EOS(EOS *e, double v) : eos(e), V(v) {}
    ~e_EOS() {}
    double f(double e); // OneDFunction::f

    double e(double T,double e0); // e(V,T)
};


double e_EOS::f(double e)
{
    return eos->T(V,e);
}

double e_EOS::e(double T, double e0)
{
    double e1 = e0;
    double T1 = eos->T(V,e1);
    if( std::isnan(T1) )
        return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__, eos,
                "T not defined at init state\n");
    if( std::abs(T-T1) < 1e-8*T )
        return e1;

    e1 += eos->CV(V,e1)*(T-T1);
    T1 = eos->T(V,e1);
    if( std::isnan(T1) )
        return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__, eos,
                                        "trial outside domain\n");
    if( std::abs(T-T1) < 1e-8*T )
        return e1;
        
    double e2, T2;
    if( T > T1 )
    {
        int count;
        for( count=5; count; count--)
        {
            double Cv = eos->CV(V,e1);
            e2 = e1 + 1.0001*Cv*(T-T1);
            T2 = eos->T(V,e2);
            if( std::isnan(T2) )
                return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__,
                        eos, "trial outside domain\n");
            if( T2 >= T )
                break;
            else
            {
                e1 = e2;
                T1 = T2;
            }
        }
        if( count == 0 )
            return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__,
                        eos, "bound failed for T > T1\n");
    }
    else
    {
        int count;
        for( count=5; count; count--)
        {
            double Cv = eos->CV(V,e1);
            e2 = e1 - 1.0001*Cv*(T1-T);
                // Thermodynamics: Cv->0 as T->0
                // e2 may lie below value on 0 isotherm
            T2 = eos->T(V,e2);
            if( std::isnan(T2) )
                return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__,
                        eos, "trial outside domain\n");
            if( T2 <= T )
                break;
            else
            {
                e1 = e2;
                T1 = T2;
            }
        }
        if( count == 0 )
            return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__,
                        eos, "bound failed for T < T1\n");
    }
        
    double e = inverse(T, e1,T1, e2,T2);
    if( Status() )
        return eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__, eos,
                    "inverse failed with status: %s\n", ErrorStatus() );
    return e;
}

//

int EOS::PT(double p, double t, HydroState &state)
{
    double Tref = T(V_ref, e_ref);
    if( std::isnan(Tref) )
    {
        EOSerror->Log("EOS::PT", __FILE__, __LINE__, this, "Tref=NaN\n");
        return 1;
    }
    if( p <= P_vac )
    {
        EOSerror->Log("EOS::PT", __FILE__, __LINE__, this, "p <= P_vac\n");
        return 1;
    }
    int status;
    double Vref = V_ref;
    double eref = e_ref;
    ThermalState thermal;
    if( t < Tref && p > P(V_ref, e_ref) )
    {
        state.V = Vref;
        state.e = e_ref;  
        Isotherm *I = isotherm(state);
        if( I == NULL )
        {
            EOSerror->Log("EOS::PT", __FILE__, __LINE__, this,
                           "isotherm returned NULL\n");
            return 1;
        }
        status = I->P(p,thermal);
        delete I;
        if( status )
        {
            EOSerror->Log("EOS::PT",__FILE__,__LINE__,this,
                          "isotherm->P failed\n");
            return 1;
        }
        else
        {
            Vref = thermal.V;
            eref = thermal.e;
        }
    }        
    e_EOS e_V(this, Vref); 
    double e_T = e_V.e(t,eref);
    if( std::isnan(e_T) )
    {
        EOSerror->Log("EOS::PT",__FILE__,__LINE__,this,
                      "e_V.e() failed\n");
        return 1;
    }
    state.V = Vref;
    state.e = e_T;  
    Isotherm *I = isotherm(state);
    if( I == NULL )
    {
        EOSerror->Log("EOS::PT",__FILE__,__LINE__,this,
                      "isotherm returned NULL\n");
        return 1;
    }
    status = I->P(p,thermal);
    delete I;
    if( status )
    {
        EOSerror->Log("EOS::PT",__FILE__,__LINE__,this,"isotherm->P failed\n");
        return 1;
    }
    state.V = thermal.V;
    state.e = thermal.e;
    // Refine with Newton-Raphson iteration
    int count=10;
    double eps = 0.01;
    while( count-- > 0 )
    {
       double V1 = state.V;
       double e1 = state.e;
       double P1 = P(V1,e1);
       double T1 = T(V1,e1);
       double dP = p - P1;
       double dT = t - T1;
       if( std::abs(dP) < OneDFunc_abs_tol+OneDFunc_rel_tol*P1
	   && std::abs(dT) < OneDFunc_abs_tol+OneDFunc_rel_tol*T1 )
           break;
       double dPde = Gamma(V1,e1)/V1;
       double dTde =  1./CV(V1,e1);
       double dPdV = dPde*P1 - c2(V1,e1)/(V1*V1);
       double dTdV = dTde*P1 - dPde*T1;
       double det = dPdV*dTde-dPde*dTdV;
       double dV = (dTde*dP - dPde*dT)/det;
       double de = (dPdV*dT - dTdV*dP)/det;
       // det = (T/V^2)(Gamma^2- gamma*g) ~ 0 in mixed region
       // for example, sesame table vapor dome near ambient state
       if( det<0.0 && std::abs(dV) < 0.05*V1 && std::abs(de) < t/dTde )
       {
           state.V += dV;
           state.e += de;
           status = 0;
       }
       else
       {
           state.V = (1-eps)*V1;
           state.e = e1;
           eps *= 0.1;
           status = 1;
       }
    }
    if( !status )
        status = NotInDomain(state.V,state.e);
    return status;
}
