#include "PTequilibrium.h"

int PTequilibrium::NotDomain1(double V, double e)
{
    if( V <= 0. )
        return 1;
    double T = eos1->T(V,e);
    if( std::isnan(T) || T<=0.0 )
        return 1;
    double P = eos1->P(V,e);
    if( std::isnan(P) || P<=0.0 )
        return 1;
    double c2    = eos1->c2(V,e);
    double Gamma = eos1->Gamma(V,e);
    double CV    = eos1->CV(V,e);
    if( std::isnan(c2) || std::isnan(Gamma) || std::isnan(CV) )
        return 1;
    /**/
    if( c2 - Gamma*Gamma*CV*T <= 0 ) // KT <= 0
        return 1;
    /**/
    V1 = V;
    e1 = e;
    P1 = P;
    T1 = T;
    c2_1 = c2;
    Gamma1 = Gamma;
    CV1 = CV;
    return 0;
}
int PTequilibrium::NotDomain2(double V, double e)
{
    if( V <= 0. )
        return 1;
    double T = eos2->T(V,e);
    if( std::isnan(T) || T<=0.0 )
        return 1;
    double P = eos2->P(V,e);
    if( std::isnan(P) || P<=0.0 )
        return 1;
    double c2    = eos2->c2(V,e);
    double Gamma = eos2->Gamma(V,e);
    double CV    = eos2->CV(V,e);
    if( std::isnan(c2) || std::isnan(Gamma) || std::isnan(CV) )
        return 1;
    /**/
    if( c2 - Gamma*Gamma*CV*T <= 0 ) // KT <= 0
        return 1;
    /**/
    V2 = V;
    e2 = e;
    P2 = P;
    T2 = T;
    c2_2 = c2;
    Gamma2 = Gamma;
    CV2 = CV;
    return 0;
}

int PTequilibrium::PTiter(double V, double e)
{
    //  Note: Analytic EOS may fail in expansion
    //        Equilibium state may not exist or may not be unique
    e += lambda2*de;
    if( Vav==V && eav==e )
        return 0;       // use cache
    if( eos1==NULL || eos2==NULL || V <= 0. )
        return 1;       
    double V1_sve = V1;
    double e1_sve = e1;
    double V2_sve = V2;
    double e2_sve = e2;
    int stat = -1;
    if( lambda1 > lambda1_max )
    { // eos1 dominated
        V1 = V;
        e1 = e;
        T1 = T2 = -1.;
        // first try with old V2,e2
        if( (stat=PT1iter(V,e)) )
        {  // second try
            V1 = V;
            e1 = e;
            T1 = T2 = -1.;
            if( NotDomain2(V,e) )
            {
                V2 = eos2->V_ref;
                e2 = eos2->e_ref;
            }
            stat = PT1iter(V,e);  
        }
    }
    else if( lambda2 > lambda2_max )
    { // eos2 dominated
        V2 = V;
        e2 = e;
        T1 = T2 = -1.;
        // first try with old V1,e1
        if( (stat=PT2iter(V,e)) )
        {  // second try
            V2 = V;
            e2 = e;
            T1 = T2 = -1.;
            if( NotDomain1(V,e) )
            {
                V1 = eos1->V_ref;
                e1 = eos1->e_ref;
            }
            stat = PT2iter(V,e);  
        }
    }
    if( stat )
    {
        if( stat > 0 )
        {
            V1 = V1_sve;
            e1 = e1_sve;
            V2 = V2_sve;
            e2 = e2_sve;
        }
        // first try with old V1,e1 and V2,e2
        T1 = T2 = -1.;
        if( (stat=PT0iter(V,e)) )
        {   // second try
            T1 = T2 = -1.;
            if( NotDomain1(V,e) )
            {
                V1 = eos1->V_ref;
                e1 = eos1->e_ref;
            }
            if( NotDomain2(V,e) )
            {
                V2 = eos2->V_ref;
                e2 = eos2->e_ref;
            }
            stat = PT0iter(V,e);
        }
    }
    if( stat )
    {
        V1 = V1_sve;
        e1 = e1_sve;
        V2 = V2_sve;
        e2 = e2_sve;
        return 1;
    }
    Vav = V;
    eav = e;
    Peq = lambda1*P1 + lambda2*P2;
    Teq = lambda1*T1 + lambda2*T2;
    return 0;
}

int PTequilibrium::PT1iter(double V, double e)
{   // lambda1 > lambda1_max
    if( NotDomain1(V1,e1) )
        return 1;
    if( NotDomain2(V2,e2) )
        return 1;
    for( count=1; count<max_iter1; count++)
    {
        double dPde2 = Gamma2/V2;
        double dTde2 = 1./CV2;
        double dPdV2 = dPde2*P2 - c2_2/(V2*V2);
        double dTdV2 = dTde2*P2 - dPde2*T2;
        double dP = P1 - P2;
        double dT = T1 - T2;
        double J = dPdV2*dTde2 -dPde2*dTdV2;
        double dV2 = (dTde2*dP - dPde2*dT)/J;
        double de2 = (dPdV2*dT - dTdV2*dP)/J;
        //
        if( NotDomain2(V2+dV2,e2+de2) )
        {
            // equilibrate T
            if( NotDomain2(V2,e2+dT/dTde2) )
                break;
            // equilibrate P
            dPde2 = Gamma2/V2;
            dPdV2 = dPde2*P2 - c2_2/(V2*V2);
            dV2 = (P1-P2)/dPdV2;
            double f = 1.0;
            if( dV2 > 0.1*V2 )
                f = 0.1*V2/dV2;            // limit expansion
            else if( dV2 < -0.05*V2 )
                f = -0.05*V2/dV2;          // limit compression
            int i;
            for( i=4; i; i--,f*=0.5 )
            {
                if( ! NotDomain2(V2+f*dV2,e2) )
                    break;
            }
            /***
            if( i == 0 )
                break;
            ***/
        }
        V1 = (V - lambda2*V2)/lambda1;
        e1 = (e - lambda2*e2)/lambda1;
        if( NotDomain1(V1,e1) )
            break;    
        if( abs(P1-P2)<tol*P1+tol_P && abs(T1-T2)<tol*T1+tol_T )
            return 0;
    }
    if( abs(P1-P2)<0.1*P1 && abs(T1-T2)<0.1*T1 )
    {
        return PT0iter(V,e);
    }
    if( verbose )
    {
        verbose = 0;
        EOSerror->Log("PTequilibrium::PT1iter",__FILE__,__LINE__,this,
                        "failed for V=%lg,e=%lg\n"
                        "lambda1=%lg,V1=%lg,e1=%lg,P1=%lg,T1=%lg\n"
                        "lambda2=%lg,V2=%lg,e2=%lg,P2=%lg,T2=%lg\n",
                        V,e, lambda1,V1,e1,P1,T1, lambda2,V2,e2,P2,T2);
    }
    T1 = T2 = -1.;  // invalid state cache
    return 1;
}

int PTequilibrium::PT2iter(double V, double e)
{   // lambda2 > lambda2_max
    if( NotDomain2(V2,e2) )
        return 1;
    if( NotDomain1(V1,e1) )
        return 1;
    for( count=1; count<max_iter1; count++)
    {
        double dPde1 = Gamma1/V1;
        double dTde1 = 1./CV1;
        double dPdV1 = dPde1*P1 - c2_1/(V1*V1);
        double dTdV1 = dTde1*P1 - dPde1*T1;
        double dP = P2 - P1;
        double dT = T2 - T1;
        double J = dPdV1*dTde1 -dPde1*dTdV1;
        double dV1 = (dTde1*dP - dPde1*dT)/J;
        double de1 = (dPdV1*dT - dTdV1*dP)/J;
        //
        if( NotDomain1(V1+dV1,e1+de1) )
        {
            // equilibrate T
            if( NotDomain1(V1,e1+dT/dTde1) )
                break;
            // equilibrate P
            dPde1 = Gamma1/V1;
            dPdV1 = dPde1*P1 - c2_1/(V1*V1);
            dV1 = (P2-P1)/dPdV1;
            double f = 1.0;
            if( dV1 > 0.1*V1 )
                f = 0.1*V1/dV1;            // limit expansion
            else if( dV1 < -0.05*V1 )
                f = -0.05*V1/dV1;          // limit compression
            int i;
            for( i=4; i; i--,f*=0.5 )
            {
                if( ! NotDomain1(V1+f*dV1,e1) )
                    break;
            }
            /***
            if( i == 0 )
                break;
            ***/
        }        
        V2 = (V - lambda1*V1)/lambda2;
        e2 = (e - lambda1*e1)/lambda2;
        if( NotDomain2(V2,e2) )
            break;    
        if( abs(P1-P2)<tol*P2+tol_P && abs(T1-T2)<tol*T2+tol_T )
            return 0;
    }
    if( abs(P1-P2)<0.1*P2 && abs(T1-T2)<0.1*T2 )
    {
        return PT0iter(V,e);
    }
    if( verbose )
    {
        verbose = 0;
        EOSerror->Log("PTequilibrium::PT2iter",__FILE__,__LINE__,this,
                        "failed for V=%lg,e=%lg\n"
                        "lambda1=%lg,V1=%lg,e1=%lg,P1=%lg,T1=%lg\n"
                        "lambda2=%lg,V2=%lg,e2=%lg,P2=%lg,T2=%lg\n",
                        V,e, lambda1,V1,e1,P1,T1, lambda2,V2,e2,P2,T2);
    }
    T1 = T2 = -1.;  // invalid state cache
    return 1;
}

int PTequilibrium::PT0iter(double V, double e)
{
    if( T1 < 0. && NotDomain1(V1,e1) )
        return 1;
    if( T2 < 0. && NotDomain2(V2,e2) )
        return 1;
    // Newton-Raphson iteration: 4 equations for V1,e1, V2,e2
    double lambdai[2];
    int i1 = (lambda1>lambda2) ? 0 : 1;
    int i2 = 1 - i1;
    lambdai[i1] = lambda1;
    lambdai[i2] = lambda2;
    double Pi[2], dPdVi[2], dPdei[2];
    double Ti[2], dTdVi[2], dTdei[2];
    for( count=1; count<max_iter; count++)
    {
        // reorder with larger lambda first
             Pi[i1] = P1;
             Ti[i1] = T1;
          dPdei[i1] = Gamma1/V1;
          dTdei[i1] = 1./CV1;
          dPdVi[i1] = dPdei[i1]*P1 - c2_1/(V1*V1);
          dTdVi[i1] = dTdei[i1]*P1 - dPdei[i1]*T1;
             Pi[i2] = P2;
             Ti[i2] = T2;
          dPdei[i2] = Gamma2/V2;
          dTdei[i2] = 1./CV2;
          dPdVi[i2] = dPdei[i2]*P2 - c2_2/(V2*V2);
          dTdVi[i2] = dTdei[i2]*P2 - dPdei[i2]*T2;
        // A*x = y
        double A[4][4];
        double x[4], y[4];
        // A[0]:  V = lambda_1*V1 + lambda2*V2
        y[0] = V - (lambda1*V1+lambda2*V2);
        A[0][0] = lambdai[0];
        A[0][1] = lambdai[1];
        //A[0][2] = A[0][3] = 0.0;
        // A[1]: P2 = P1
        y[1] = Pi[1]-Pi[0];
        A[1][0] =  dPdVi[0];
        A[1][1] = -dPdVi[1];
        A[1][2] =  dPdei[0];
        A[1][3] = -dPdei[1];
        // A[2]:  e = lambda_1*e1 + lambda2*e2
        y[2] = e - (lambda1*e1+lambda2*e2);
        //A[2][0] = A[2][1] = 0.0;
        A[2][2] = lambdai[0];
        A[2][3] = lambdai[1];
        // A[3]: T2 = T1
        y[3] = Ti[1]-Ti[0];
        A[3][0] =  dTdVi[0];
        A[3][1] = -dTdVi[1];
        A[3][2] =  dTdei[0];
        A[3][3] = -dTdei[1];
        // Gaussian elimination
        double r;
        r = A[1][0]/A[0][0];
        //A[1][0]  = 0.0;
        A[1][1] -= r*A[0][1];
        y[1]    -= r*y[0];
        r = A[3][0]/A[0][0];
        //A[3][0]  = 0.0;
        A[3][1] -= r*A[0][1];
        y[3]    -= r*y[0];
        r = A[3][1]/A[1][1];
        //A[3][1]  = 0.0;
        A[3][2] -= r*A[1][2];
        A[3][3] -= r*A[1][3];
        y[3]    -= r*y[1];
        r = A[3][2]/A[2][2];
        //A[3][2]  = 0.0;
        A[3][3] -= r*A[2][3];
        y[3]    -= r*y[2];
        // backwards substitution
        x[3] = y[3]/A[3][3];
        x[2] = (y[2]-A[2][3]*x[3])/A[2][2];
        x[1] = (y[1]-A[1][3]*x[3]-A[1][2]*x[2])/A[1][1];
        x[0] = (y[0]-A[0][1]*x[1])/A[0][0];
        // Update
        double dV1 = x[i1];
        double de1 = x[i1+2];
        // Check domain
        double f1 = 1.0;
        if( dV1 > 0.1*V1 )
            f1 = 0.1*V1/dV1;            // limit expansion
        else if( dV1 < -0.05*V1 )
            f1 = -0.05*V1/dV1;          // limit compression
        int i;
        for( i=4; i; i--,f1*=0.5 )
        {
            if( ! NotDomain1(V1+f1*dV1,e1+f1*de1) )
                break;
        }
        double dV2 = x[i2];
        double de2 = x[i2+2];
        double f2 = 1.0;
        if( dV2 > 0.1*V2 )
            f2 = 0.1*V2/dV2;            // limit expansion
        else if( dV2 < -0.05*V2 )
            f2 = -0.05*V2/dV2;          // limit compression
        int j;
        for( j=4; j; j--,f2*=0.5 )
        {
            if( ! NotDomain2(V2+f2*dV2,e2+f2*de2) )
                break;
        }
        if( j==0 && i==0 )
        {
            // raise lower temperature to match higher one
            if( T1 > T2 )
            {
                if( NotDomain2(V2,e2+(T1-T2)*CV2 ) )
                    break;              
            }
            else
            {
                if( NotDomain1(V1,e1+(T2-T1)*CV1 ) )
                    break;              
            }
        }
        if( f1==1.0 && f2==1.0 )
            break;
    }
    // V = lambda1*V1 + lambda2*V2
    // e = lambda1*e1 + lambda2*e2
    // reduced iteration on 2 variables V1,e1 for P & T equilibrium
    double r = lambdai[1]/lambdai[0];
    while( count++ < max_iter )
    {
        // reorder with larger lambda first
             Pi[i1] = P1;
             Ti[i1] = T1;
          dPdei[i1] = Gamma1/V1;
          dTdei[i1] = 1./CV1;
          dPdVi[i1] = dPdei[i1]*P1 - c2_1/(V1*V1);
          dTdVi[i1] = dTdei[i1]*P1 - dPdei[i1]*T1;
             Pi[i2] = P2;
             Ti[i2] = T2;
          dPdei[i2] = Gamma2/V2;
          dTdei[i2] = 1./CV2;
          dPdVi[i2] = dPdei[i2]*P2 - c2_2/(V2*V2);
          dTdVi[i2] = dTdei[i2]*P2 - dPdei[i2]*T2;
        //
        double dPdV = r*dPdVi[0] + dPdVi[1];
        double dPde = r*dPdei[0] + dPdei[1];
        double dTdV = r*dTdVi[0] + dTdVi[1];
        double dTde = r*dTdei[0] + dTdei[1];
        double dP   = Pi[0]-Pi[1];
        double dT   = Ti[0]-Ti[1];
        double J = dPdV*dTde -dPde*dTdV;
        double dV = (dTde*dP - dPde*dT)/J;
        double de = (dPdV*dT - dTdV*dP)/J;
        // Update
        double V1_0 = V1;
        double e1_0 = e1;
        double V2_0 = V2;
        double e2_0 = e2;
        if( i1 == 1 )
        {
            V1 += dV;
            e1 += de;
            V2 = (V-lambda1*V1)/lambda2;
            e2 = (e-lambda1*e1)/lambda2;
        }
        else
        {
            V2 += dV;
            e2 += de;
            V1 = (V-lambda2*V2)/lambda1;
            e1 = (e-lambda2*e2)/lambda1;
        }
        if( NotDomain1(V1,e1) || NotDomain2(V2,e2) )
        {
            // equilibrate T
            de = dT/dTde;
            if( i1 == 1 )
            {
                e1 = e1_0+de;
                e2 = (e-lambda1*e1)/lambda2;
            }
            else
            {
                e2 = e2_0+de;
                e1 = (e-lambda2*e2)/lambda1;
            }
            if( NotDomain1(V1_0,e1) || NotDomain2(V2_0,e2) )
                break;
            // equilibrate P
            dPdVi[i1] = Gamma1/V1*P1-c2_1/(V1*V1);
            dPdVi[i2] = Gamma2/V2*P2-c2_2/(V2*V2);
            dPdV = r*dPdVi[0] + dPdVi[1];
            double dV1;
            double dV2;
            double f = 1.0;
            if( i1 == 1 )
            {
                dV1 = (P2-P1)/dPdV;
                dV2 = -(lambda1/lambda2)*dV1;
            }
            else
            {
                dV2 = (P1-P2)/dPdV;
                dV1 = -(lambda2/lambda1)*dV2;
            }
            if( dV1 > 0.1*V1 )
                f = 0.1*V1/dV1;            // limit expansion
            else if( dV1 < -0.05*V1 )
                f = -0.05*V1/dV1;          // limit compression
            if( f*dV2 > 0.1*V2 )
                f = 0.1*V2/dV2;            // limit expansion
            else if( f*dV2 < -0.05*V2 )
                f = -0.05*V2/dV2;          // limit compression
            int i;
            for( i=4; i; i--,f*=0.5 )
            {
                if( !(NotDomain1(V1+f*dV1,e1) || NotDomain2(V2+f*dV2,e2)) )
                    break;
            }
            /***
            if( i == 0 )
                break;
            ***/
        }
        // check convergence
        if( abs(P1-P2)<tol*(lambda1*P1+lambda2*P2)+tol_P
            && abs(T1-T2)<tol*(lambda1*T1+lambda2*T2)+tol_T )
        {
            return 0;
        }
    }
    if( verbose )
    {
        verbose = 0;
        EOSerror->Log("PTequilibrium::PT0iter",__FILE__,__LINE__,this,
                        "failed for V=%lg,e=%lg\n"
                        "lambda1=%lg,V1=%lg,e1=%lg,P1=%lg,T1=%lg\n"
                        "lambda2=%lg,V2=%lg,e2=%lg,P2=%lg,T2=%lg\n",
                        V,e, lambda1,V1,e1,P1,T1, lambda2,V2,e2,P2,T2);
    }
    T1 = T2 = -1.;  // invalid state cache
    return 1;
}
