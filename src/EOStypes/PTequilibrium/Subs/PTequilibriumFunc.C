#include "PTequilibrium.h"

double PTequilibrium::P(double V, double e)
{
    if( lambda1 >= 1.0 )
        return eos1 ? eos1->P(V,e) : NaN;
    if( lambda2 >= 1.0 )
        return eos2 ? eos2->P(V,e+de) : NaN;
    return PTiter(V,e) ? NaN : Peq;
}

double PTequilibrium::T(double V, double e)
{
    if( lambda1 >= 1.0 )
        return eos1 ? eos1->T(V,e) : NaN;
    if( lambda2 >= 1.0 )
        return eos2 ? eos2->T(V,e+de) : NaN;
    return PTiter(V,e) ? NaN : Teq;
}

double PTequilibrium::S(double V, double e)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->S(V,e) : NaN;
    //if( lambda2 >= 1.0 )
    if( lambda2 >= lambda2_max )
        return eos2 ? eos2->S(V,e+de) : NaN;
    if( PTiter(V,e) )
        return NaN;
    double S1 = eos1->S(V1,e1);
    double S2 = eos2->S(V2,e2);
    return lambda1*S1 + lambda2*S2;
}

// Note:
// (dP/de)*(dT/dV) - (dP/dV)*(dT/de) = (rho c_T)^2/C_V > 0
double PTequilibrium::Gamma(double V, double e)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->Gamma(V,e) : NaN;
    //if( lambda2 >= 1.0 )
    if( lambda2 >= lambda2_max )
        return eos2 ? eos2->Gamma(V,e+de) : NaN;
    if( PTiter(V,e) )
        return NaN;
    ddVe();
    return V*( lambda1*dPde1*(dPdV2*dTde2-dPde2*dTdV2)
               + lambda2*dPde2*(dPdV1*dTde1-dPde1*dTdV1) )
           / ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) );
}

double PTequilibrium::CV(double V, double e)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->CV(V,e) : NaN;
    //if( lambda2 >= 1.0 )
    if( lambda2 >= lambda2_max )
        return eos2 ? eos2->CV(V,e+de) : NaN;
    if( PTiter(V,e) )
        return NaN;
    ddVe();
    return ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) )
           / ( lambda1*dTde1*(dPdV2*dTde2-dPde2*dTdV2)
               + lambda2*dTde2*(dPdV1*dTde1-dPde1*dTdV1) );
}

double PTequilibrium::c2(double V, double e)
{
    if( lambda1 >= 1.0 )
        return eos1 ? eos1->c2(V,e) : NaN;
    if( lambda2 >= 1.0 )
        return eos2 ? eos2->c2(V,e+de) : NaN;
    double C2 = -1.;
    if( ! PTiter(V,e) )
    {
        ddVe();
        C2 = V*V*( lambda1*(Peq*dPde1-dPdV1)*(dPdV2*dTde2-dPde2*dTdV2)
            +lambda2*(Peq*dPde2-dPdV2)*(dPdV1*dTde1-dPde1*dTdV1) )
            / ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) );
         if( C2 > 0.0 ) return C2;
    }
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->c2(V,e) : NaN;
    if( lambda2 >= lambda2_max )
        return eos2 ? eos2->c2(V,e+de) : NaN;
    return NaN;
}

double PTequilibrium::FD(double V, double e)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->FD(V,e) : NaN;
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos2 ? eos2->FD(V,e+de) : NaN;
    return EOS::FD(V,e);
}

double PTequilibrium::e_PV(double p, double V)
{
    if( lambda1 >= 1.0 )
        return eos1 ? eos1->e_PV(p,V) : NaN;
    if( lambda2 >= 1.0 )
        return eos2 ? eos2->e_PV(p,V)-de : NaN;
    return EOS::e_PV(p,V);
}


int PTequilibrium::NotInDomain(double V, double e)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->NotInDomain(V,e) : 1;
    //if( lambda2 >= 1.0 )
    if( lambda2 >= lambda2_max )
        return eos2 ? eos2->NotInDomain(V,e+de) : 1;
    double ceq2 = c2(V,e);
    return isnan(ceq2) || ceq2 <= 0;
}

int PTequilibrium::PT(double P, double T, HydroState &state)
{
    //if( lambda1 >= 1.0 )
    if( lambda1 >= lambda1_max )
        return eos1 ? eos1->PT(P,T,state) : 1;
    //if( lambda2 >= 1.0 )
    if( lambda2 >= lambda2_max )
    {
        if( eos2 == NULL ) return 1;
        int status = eos2->PT(P,T,state);
        if( !status ) state.e -= de;
        return status;
    }
    return EOS::PT(P,T,state);
}
//
//
//
void PTequilibrium::set_lambda1(double lambda)
{
    lambda1 = min(max(0.,lambda),1.);
    lambda2 = 1. - lambda1;
    Vav = eav = -1.23456;
}
void PTequilibrium::set_lambda1(double lambda, double q)
{
    lambda1 = min(max(0.,lambda),1.);
    lambda2 = 1. - lambda1;
    de = q;
    Vav = eav = -1.23456;
}
void PTequilibrium::set_lambda2(double lambda)
{
    lambda2 = min(max(0.,lambda),1.);
    lambda1 = 1. - lambda2;
    Vav = eav = -1.23456;
}
void PTequilibrium::set_lambda2(double lambda, double q)
{
    lambda2 = min(max(0.,lambda),1.);
    lambda1 = 1. - lambda2;
    de = q;
    Vav = eav = -1.23456;
}

int PTequilibrium::ddlambda(double V, double e, double &dP, double &dT)
{
    if( PTiter(V,e) )
        return 1;
    ddVe();
    double det = (lambda1*dTdV2+lambda2*dTdV1)*(lambda1*dPde2+lambda2*dPde1)
                -(lambda1*dPdV2+lambda2*dPdV1)*(lambda1*dTde2+lambda2*dTde1);
    dP = ( lambda1*((V2-V1)*dPdV1+(e2-e1)*dPde1)*(dTdV2*dPde2-dPdV2*dTde2)
          +lambda2*((V2-V1)*dPdV2+(e2-e1)*dPde2)*(dTdV1*dPde1-dPdV1*dTde1)
         )/det;
    dT = ( lambda1*((V2-V1)*dTdV1+(e2-e1)*dTde1)*(dTdV2*dPde2-dPdV2*dTde2)
           +lambda2*((V2-V1)*dTdV2+(e2-e1)*dTde2)*(dTdV1*dPde1-dPdV1*dTde1)
         )/det;
    if( de != 0. )
    {
    // P(V,e,lambda) = Peq(V, e+(1-lambda)*de, lambda)
    // (dP/dlambda)_V,e = (dPeq/dlambda)_V,e - de*(dPeq/de)_V,lambda
        double GammaV = ( lambda1*dPde1*(dPdV2*dTde2-dPde2*dTdV2)
                    + lambda2*dPde2*(dPdV1*dTde1-dPde1*dTdV1) )
               / ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) );
    // T(V,e,lambda) = Teq(V, e+(1-lambda)*de, lambda)
    // (dT/dlambda)_V,e = (dTeq/dlambda)_V,e - de*(dTeq/de)_V,lambda
        double Cv = ( (lambda2*dPdV1+lambda1*dPdV2)*(lambda2*dTde1+lambda1*dTde2)
               -(lambda2*dPde1+lambda1*dPde2)*(lambda2*dTdV1+lambda1*dTdV2) )
             / ( lambda1*dTde1*(dPdV2*dTde2-dPde2*dTdV2)
                 + lambda2*dTde2*(dPdV1*dTde1-dPde1*dTdV1) );
        dP -= de*GammaV;
        dT -= de*Cv;
    }
    return 0;
}
