#include "StiffIdeal_VT.h"

int StiffIdeal_VT::Pequil(double V, double T)
{
    Vav = V;
    Teq = T;
    if( lambda1 >= 1. )
    {
        V1 = V;
        Peq = G1*Cv1*T/V - P_p;
        V2 = G2*Cv2*T/Peq;
    }
    else if( lambda2 >= 1. )
    {
        V2 = V;
        Peq = G2*Cv2*T/V;
        V1 = G1*Cv1*T/(Peq+P_p);
    }
    else
    {
        double a0 = G1*Cv1*T*V;
        double a1 = 0.5*((lambda1*G1*Cv1+lambda2*G2*Cv2)*T + P_p*V);
        if( P_p == 0 )
        {
            V1 = G1*Cv1*V/(lambda1*G1*Cv1+lambda2*G2*Cv2);
            V2 = G2*Cv2*V/(lambda1*G1*Cv1+lambda2*G2*Cv2);
            Peq = G2*Cv2*T/V2;
        }
        else
        {
            double a2 = P_p;
            if( lambda1 > 0.5 )
            {
                a2 *= lambda1;
                V1 = (a1-sqrt(a1*a1-a0*a2))/a2;
                Peq = G1*Cv1*T/V1 - P_p;
                V2 = G2*Cv2*T/Peq;
            }
            else
            {
                // V1 -> lambda1*V1
                V1 = (a1-sqrt(a1*a1-lambda1*a0*a2))/a2;
                V2 = (V - V1)/lambda2;
                Peq = G2*Cv2*T/V2;
                V1 = G1*Cv1*T/(Peq+P_p);
            }
        }
    }
    if( Peq<0 )
    {
        Teq = -1;
        Vav = 0.0;
        return 1;
    }
    e1 = Cv1*T +P_p*V1;
    e2 = Cv2*T-Q;
    eav = lambda1*e1+lambda2*e2;
    return 0;
}

double StiffIdeal_VT::P(double V, double T)
{
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    return Peq;
    
}

double StiffIdeal_VT::e(double V, double T)
{
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    return eav;
}

double StiffIdeal_VT::S(double V, double T)
{
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    double S1 = Cv1*(log((e1-P_p*V1)/(e_ref-P_p*V_ref))
                     + G1*log(V1/V_ref));
    double S2 = Cv2*(log(e2/e_ref)+ G2*log(V2/V_ref)) + dS;
    //double S1 = Cv1*( (G1+1.)*log(Teq/T_ref)-G1*log((Peq+P_p)/(Pref+P_p)) );
    //double S2 = Cv2*( (G2+1.)*log(Teq/T_ref)-G2*log(Peq/Pref) ) + dS;
    return lambda1*S1 + lambda2*S2;    
}

double StiffIdeal_VT::P_V(double V, double T)
{
    if( lambda1 >= 1. )
        return -G1*Cv1*T/(V*V);
    if( lambda2 >= 1. )
        return -G2*Cv2*T/(V*V);
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    double dPdV1 = G1*Cv1*T/(V1*V1);
    double dPdV2 = G2*Cv2*T/(V2*V2);
    return -dPdV1*dPdV2/(lambda2*dPdV1+lambda1*dPdV2);
}

double StiffIdeal_VT::P_T(double V, double T)
{
    if( lambda1 >= 1. )
        return G1*Cv1/V;
    if( lambda2 >= 1. )
        return G2*Cv2/V;
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    double dPdV1 = lambda2*G1*Cv1*T/(V1*V1);
    double dPdV2 = lambda1*G2*Cv2*T/(V2*V2);
    double dPdT1 = G1*Cv1/V1;
    double dPdT2 = G2*Cv2/V2;
    return (dPdV1*dPdT2+dPdV2*dPdT1)/(dPdV1+dPdV2);
}

double StiffIdeal_VT::CV(double V, double T)
{
    if( lambda1 >= 1. )
        return Cv1;
    if( lambda2 >= 1. )
        return Cv2;
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    double dPdV1 = lambda2*G1*Cv1*T/(V1*V1);
    double dPdV2 = lambda1*G2*Cv2*T/(V2*V2);
    return (dPdV1*Cv2+dPdV2*Cv1)/(dPdV1+dPdV2);
}

double StiffIdeal_VT::Gamma(double V, double T)
{
    if( lambda1 >= 1. )
        return G1;
    if( lambda2 >= 1. )
        return G2;
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;
    // Gamma/V = (dP/de|V) = (dP/dT|V) / (de/dT|V) = P_T/CV
    double dPdV1 = lambda2*G1*Cv1*T/(V1*V1);
    double dPdV2 = lambda1*G2*Cv2*T/(V2*V2);
    double dPdT1 = G1*Cv1/V1;
    double dPdT2 = G2*Cv2/V2;
    return V*(dPdV1*dPdT2+dPdV2*dPdT1)/(dPdV1*Cv2+dPdV2*Cv1);  
}

double StiffIdeal_VT::c2(double V, double T)
{
    if( lambda1 >= 1. )
        return (G1+1.)*G1*Cv1*T;
    if( lambda2 >= 1. )
        return (G2+1.)*G2*Cv2*T;
    if( V != Vav || T != Teq )
        if( Pequil(V,T) )
            return NaN;

    double dPdV1 = -G1*Cv1*T/(V1*V1);
    double dPdV2 = -G2*Cv2*T/(V2*V2);
    double dPdT1 = G1*Cv1/V1;
    double dPdT2 = G2*Cv2/V2;

    double Peq_V = dPdV1*dPdV2;
    dPdV1 *= lambda2;
    dPdV2 *= lambda1;
    double Peq_T = dPdV1*dPdT2+dPdV2*dPdT1;
    double CVeq  = dPdV1*Cv2+dPdV2*Cv1;

    return V*V*(-Peq_V+(T/CVeq)*sqr(Peq_T))/(dPdV1+dPdV2);    
}

int StiffIdeal_VT::NotInDomain(double V, double T)
{
    if( V == Vav && T == Teq )
        return 0;
    return Pequil(V,T);
}
