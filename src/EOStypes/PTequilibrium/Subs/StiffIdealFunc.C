#include <StiffIdeal.h>

int StiffIdeal::PTequil(double V, double e)
{
    Vav = V;
    eav = e;
    if( lambda1 >= 1. )
    {
        Peq = G1*e/V -(G1+1.)*P_p;
        Teq = (e-P_p*V)/Cv1;
    }
    else if( lambda2 >= 1. )
    {
        Peq = G2*(e+Q)/V;
        Teq = (e+Q)/Cv2;
    }
    else
    {
        if( P_p == 0. )
        {   // Stiffened gas reduces to ideal gas
            Teq = (e+lambda2*Q)/(lambda1*Cv1+lambda2*Cv2);
            Peq = (G1*lambda1*Cv1+G2*lambda2*Cv2)*Teq/V;
        }
        else
        {
            double a2 = (lambda1*Cv1+lambda2*Cv2)*V;
            double a1 = 0.5*((G1*lambda1*Cv1+G2*lambda2*Cv2)*(e+lambda2*Q)
                             -((G1+1.)*lambda1*Cv1+lambda2*Cv2)*P_p*V);
            double a0 = G2*lambda2*Cv2*(e+lambda2*Q)*P_p;
            // a2*Peq^2 -2*a1*Peq - a0 = 0
            Peq = (a1 + sqrt(a1*a1+a0*a2))/a2;
            Teq = V/(G1*lambda1*Cv1/(Peq+P_p)+G2*lambda2*Cv2/Peq);
        }
    }
    if( Peq < 0. || Teq < 0. )
    {
        Vav = -1.0;
        eav = 0.0;
        return 1;
    }
    V1 = G1*Cv1*Teq/(Peq+P_p);
    V2 = G2*Cv2*Teq/Peq;
    e1 = Cv1*Teq + P_p*V1;
    e2 = Cv2*Teq-Q;
    return 0;
}

double StiffIdeal::P(double V, double e)
{
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    return Peq;
}

double StiffIdeal::T(double V, double e)
{
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    return Teq;
}

double StiffIdeal::S(double V, double e)
{
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    double S1 = Cv1*(log((e1-P_p*V1)/(e_ref-P_p*V_ref))
                     + G1*log(V1/V_ref));
    double S2 = Cv2*(log((e2+Q)/(e_ref+Q))+ G2*log(V2/V_ref)) + dS;

    //double S1 = Cv1*( (G1+1.)*log(Teq/T_ref)-G1*log((Peq+P_p)/(Pref+P_p)) );
    //double S2 = Cv2*( (G2+1.)*log(Teq/T_ref)-G2*log(Peq/Pref) ) + dS;
    return lambda1*S1 + lambda2*S2;
}

double StiffIdeal::c2(double V, double e)
{
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    if( lambda1 >= 1. )
        return (G1+1.)*(Peq+P_p)*V1;
    if( lambda2 >= 1. )
        return (G2+1.)*Peq*V2;
    //
    double dPde1 = G1/V1;
    double dPdV1 = -G1*e1/(V1*V1);
    double dTde1 = 1./Cv1;
    double dTdV1 = -P_p/Cv1;
    //
    double dPde2 = G2/V2;
    double dPdV2 = -Peq/V2;
    double dTde2 = 1./Cv2;
    double dTdV2 = 0.0;

    return V*V*( lambda1*(Peq*dPde1-dPdV1)*(dPdV2*dTde2-dPde2*dTdV2)
            +lambda2*(Peq*dPde2-dPdV2)*(dPdV1*dTde1-dPde1*dTdV1) )
         / ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
            -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) );
}

double StiffIdeal::Gamma(double V, double e)
{
    if( lambda1 >= 1. )
        return G1;
    if( lambda2 >= 1. )
        return G2;
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    //
    double dPde1 = G1/V1;
    double dPdV1 = -G1*e1/(V1*V1);
    double dTde1 = 1./Cv1;
    double dTdV1 = -P_p/Cv1;
    //
    double dPde2 = G2/V2;
    double dPdV2 = -Peq/V2;
    double dTde2 = 1./Cv2;
    double dTdV2 = 0.0;

    return V*( lambda1*dPde1*(dPdV2*dTde2-dPde2*dTdV2)
               + lambda2*dPde2*(dPdV1*dTde1-dPde1*dTdV1) )
           / ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) );
}

double StiffIdeal::CV(double V, double e)
{
    if( lambda1 >= 1. )
        return Cv1;
    if( lambda2 >= 1. )
        return Cv2;
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return NaN;
    //
    double dPde1 = G1/V1;
    double dPdV1 = -G1*e1/(V1*V1);
    double dTde1 = 1./Cv1;
    double dTdV1 = -P_p/Cv1;
    //
    double dPde2 = G2/V2;
    double dPdV2 = -Peq/V2;
    double dTde2 = 1./Cv2;
    double dTdV2 = 0.0;

    return ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) )
           / ( lambda1*dTde1*(dPdV2*dTde2-dPde2*dTdV2)
               + lambda2*dTde2*(dPdV1*dTde1-dPde1*dTdV1) );
}

double StiffIdeal::FD(double V, double e)
{
    if( lambda1 >= 1. )
        return 0.5*G1+1.;
    if( lambda2 >= 1. )
        return 0.5*G2+1.;
    return EOS::FD(V,e);
}

int StiffIdeal::NotInDomain(double V, double e)
{
    if( V == Vav && e == eav )
        return 0;
    return PTequil(V,e);
}

int StiffIdeal::PT(double P, double T, HydroState &state)
{
    Peq = P;
    Teq = T;
    V1 = G1*Cv1*Teq/(Peq+P_p);
    e1 = Cv1*Teq + P_p*V1;
    V2 = G2*Cv2*Teq/Peq;
    e2 = Teq*Cv2-Q;
    Vav = lambda1*V1 + lambda2*V2;
    eav = lambda1*e1 + lambda2*e2;
    state.V = Vav;
    state.e = eav;
    return 0;
}

int StiffIdeal::ddlambda(double V, double e, double &dP, double &dT)
{
    if( V != Vav || e != eav )
        if( PTequil(V,e) )
            return 1;

    double V_T = (lambda1*V1+lambda2*V2)/Teq;
    double V_P = -(lambda1*V1/(Peq+P_p)+lambda2*V2/Peq);
    double e_T = lambda1*(Cv1+P_p*V1/Teq) + lambda2*Cv2;
    double e_P = -lambda1*V1*P_p/(Peq+P_p);

    double det = V_T*e_P - e_T*V_P;
    double dV = V2-V1;
    double de = e2-e1;
    dP = (V_T*de - e_T*dV)/det; // = d(Peq)/d(lambda1)
    dT = (dV*e_P - de*V_P)/det; // = d(Teq)/d(lambda1)
    return 0;
}
