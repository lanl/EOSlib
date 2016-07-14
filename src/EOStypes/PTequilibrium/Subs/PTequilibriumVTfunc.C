#include "PTequilibrium_VT.h"

double PTequilibrium_VT::P(double V, double T)
{
    if( eos1==NULL || eos2==NULL || T < 0 || V <= 0 )
        return NaN;
    if( lambda1 >= 1.0 )
        return eos1->P(V,T);
    if( lambda2 >= 1.0 )
        return eos2->P(V,T);
    //V1 = min(V,eos1->V_ref);
    //V2 = min(V,eos2->V_ref);
    V1 = eos1->V_ref;
    V2 = eos2->V_ref;
    double P1 = eos1->P(V1,T);
    double P2 = eos2->P(V2,T);
    double K1 = eos1->cT2(V1,T);
    double K2 = eos2->cT2(V2,T);
    for(count=1; count<=40; count++)
    {
      K1 /= V1*V1;
      K2 /= V2*V2;
      double Kav = lambda2*K1+lambda1*K2;
      double dV = V-lambda1*V1-lambda2*V2;
      double V2_0 = V2;
      double V1_0 = V1;
      double dV2 = (lambda1*(P1-P2)-K1*dV)/Kav;
      double dV1 = (lambda2*(P2-P1)-K2*dV)/Kav;
      if( std::abs(dV1) > 0.5*V1 )
      {
          //dV2 *= 0.5*V1/std::abs(dV1);
          dV1 = (dV1 < 0.) ? -0.5*V1 : 0.5*V1;
      }
      if( std::abs(dV2) > 0.5*V2 )
      {
          //dV1 *= 0.5*V2/std::abs(dV2);
          dV2 = (dV2 < 0.) ? -0.5*V2 : 0.5*V2;
      }
      int i;
      for( i=10; i; i-- )
      {
          V1 = V1_0 - dV1;
          if( !(std::isnan(P1=eos1->P(V1,T)) || P1 <= 0
                  || std::isnan(K1=eos1->cT2(V1,T)) || K1 <= 0) )
          {
            break;
          }
          dV1 *= 0.5;
      }
      if( i == 0 )
      {
          dV1 = 0.0;
          V1 = V1_0;
          P1=eos1->P(V1,T);
          K1=eos1->cT2(V1,T);
      }
      for( i=10; i; i-- )
      {
          V2 = V2_0 - dV2;
          if( !(std::isnan(P2=eos2->P(V2,T)) || P2 <= 0
                  || std::isnan(K2=eos2->cT2(V2,T)) || K2 <= 0) )
          {
            break;
          }
          dV2 *= 0.5;
      }
      if( i == 0 )
      {
          if( dV1 == 0 )
            return NaN;
          V2 = V2_0;
          P2=eos2->P(V2,T);
          K2=eos2->cT2(V2,T);         
      }
      if( std::abs(P1-P2) < tol*abs(P1+P2) && abs(dV) < tol*V )
          return 0.5*(P1+P2);
    }
    // V1 = V2 = 0.0;
    return NaN;
}

double PTequilibrium_VT::e(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->e(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->e(V,T) - de;
    if( std::isnan(P(V,T)) )
        return NaN;
    return lambda1*eos1->e(V1,T) + lambda2*(eos2->e(V2,T)-de);
}

double PTequilibrium_VT::S(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->S(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->S(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    return lambda1*eos1->S(V1,T) + lambda2*eos2->S(V2,T);
}

double PTequilibrium_VT::P_V(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->P_V(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->P_V(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    double dPdV1 = eos1->P_V(V1,T);
    double dPdV2 = eos2->P_V(V2,T);
    return dPdV1*dPdV2/(lambda2*dPdV1+lambda1*dPdV2);
}

double PTequilibrium_VT::P_T(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->P_T(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->P_T(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    double dPdV1 = lambda2*eos1->P_V(V1,T);
    double dPdT1 = eos1->P_T(V1,T);
    double dPdV2 = lambda1*eos2->P_V(V2,T);
    double dPdT2 = eos2->P_T(V2,T);
    return (dPdV1*dPdT2+dPdV2*dPdT1)/(dPdV1+dPdV2);
}

double PTequilibrium_VT::CV(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->CV(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->CV(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    double dPdV1 = eos1->P_V(V1,T);
    double CV1   = eos1->CV(V1,T);
    double G1    = eos1->Gamma(V1,T);
    //
    double dPdV2 = eos2->P_V(V2,T);
    double CV2   = eos2->CV(V2,T);
    double G2    = eos2->Gamma(V1,T);
    // CV = (partial e)/(partial T)_V
    return  (lambda1*CV1 + lambda2*CV2)
           - lambda1*lambda2/(lambda2*dPdV1+lambda1*dPdV2)
             * sqr(G1/V1*CV1-G2/V2*CV2)*T;
}

double PTequilibrium_VT::Gamma(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->Gamma(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->Gamma(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    // Gamma/V = (dP/de|V) = (dP/dT|V) / (de/dT|V) = P_T/CV
    double dPdV1 = eos1->P_V(V1,T);
    double dPdT1 = eos1->P_T(V1,T);
    double CV1   = eos1->CV(V1,T);
    double G1    = eos1->Gamma(V1,T);
    //
    double dPdV2 = eos2->P_V(V2,T);
    double dPdT2 = eos2->P_T(V2,T);
    double CV2   = eos2->CV(V2,T);
    double G2    = eos2->Gamma(V1,T);
    //
    double CVeq = (lambda1*CV1 + lambda2*CV2)*(lambda2*dPdV1+lambda1*dPdV2)
                  - lambda1*lambda2*sqr(G1/V1*CV1-G2/V2*CV2)*T;
    double P_Teq = lambda2*dPdV1*dPdT2+lambda1*dPdV2*dPdT1;
    return V*P_Teq/CVeq;
}

double PTequilibrium_VT::c2(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->c2(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->c2(V,T);
    if( std::isnan(P(V,T)) )
        return NaN;
    //
    double dPdV1 = eos1->P_V(V1,T);
    double dPdT1 = eos1->P_T(V1,T);
    double CV1   = eos1->CV(V1,T);
    double G1    = eos1->Gamma(V1,T);
    //
    double dPdV2 = eos2->P_V(V2,T);
    double dPdT2 = eos2->P_T(V2,T);
    double CV2   = eos2->CV(V2,T);
    double G2    = eos2->Gamma(V1,T);
    //
    double denom = lambda2*dPdV1+lambda1*dPdV2;
    double CVeq = (lambda1*CV1 + lambda2*CV2)*denom
                  - lambda1*lambda2*sqr(G1/V1*CV1-G2/V2*CV2)*T;
    double Peq_T = lambda2*dPdV1*dPdT2+lambda1*dPdV2*dPdT1;
    // c2 = cT2 + Gamma^2 * (CV*T)
    return V*V*(-dPdV1*dPdV2+sqr(Peq_T)*T/CVeq)/denom;    
}

int PTequilibrium_VT::NotInDomain(double V, double T)
{
    if( lambda1 >= 1.0 && eos1 )
        return eos1->NotInDomain(V,T);
    if( lambda2 >= 1.0 && eos2 )
        return eos2->NotInDomain(V,T);
    double Peq_V = P_V(V,T); // (rho*cT)^2 = -P_V
    return std::isnan(Peq_V) || Peq_V >= 0.0;
}
