#ifndef EOSLIB_SHOCK_POLAR_H
#define EOSLIB_SHOCK_POLAR_H

#include "EOS.h"


/**********************************************************
Ref: Henderson and Menikoff, Triple shock entropy theorem
     J. Fluid Mech. 366 (1998) pp. 179-210, Appendix B
Notation
--------
      q0 = ahead  particle speed > c0
      q1 = behind particle speed
  theta0 = angle between incoming particle path and x-axis
  theta1 = angle between outgoing particle path and x-axis
  
   beta0 = angle between incoming particle path and shock front
   beta1 = angle between outgoing particle path and shock front
            -pi/2 <= beta0,beta1 <= pi/2
            + for counter-clockwise wave
            - for clockwise wave
   beta  = angle between shock front and x-axis
         = beta0 + theta0 = beta1 + theta1
  
  Let up,us on Hugoniot locus based on ahead state with u0=0
  q_par  = q0*cos(beta0) = q1*cos(beta1)
  us     = q0*sin(|beta0|) > c0
  us-up  = q1*sin(beta1)
  dtheta = turning angle = theta1 - theta0 = beta0 - beta1
  tan(dtheta) = +- [us*up/(q0^2-us*up)]*q_par/us
            + for counter-clockwise wave
            - for clockwise wave
**********************************************************/
// dir = RIGHT ( 1) for counter-clockwise wave
//     = LEFT  (-1) for clockwise wave
// dtheta increases for CC shock
//        decreases for CC Prandtl-Meyer fan
class ShockPolar                                // abstract base class
{
private:    
    ShockPolar();                               // disallowed
    ShockPolar(const ShockPolar&);              // disallowed
    void operator=(const ShockPolar&);          // disallowed
    ShockPolar *operator &();                   // disallowed    
protected:
    ShockPolar(EOS &e);
    void Wave2Polar(WaveState &wave, int dir, PolarWaveState &polar);
public:
    double P_tol;                               // tolerance for P
    double theta_tol;                           // tolerance for theta
    EOS *eos;
    Hugoniot *hug;
    WaveState pwave;                            // hug->x(var,dir,pwave)
    PolarState state0;
    double beta0_min;                           // beta0_min < beta0 <= pi/2
    double c0;                                  // c0 = q0*sin(beta0_min)
    double P0;
    double B;                                   // B = 0.5*q^2 + e + P*V
    WaveState Pmax;                             // shock with us = q0
    PolarWaveState dtheta_max;      // computed by derived Initialize
    PolarWaveState sonic;           // computed by derived Initialize
public:
    virtual ~ShockPolar();
    virtual int Initialize(PolarState &s);
    
    virtual int     P(double p, int dir, PolarWaveState &polar);
    virtual int beta0(double b, int dir, PolarWaveState &polar);
    virtual int ThetaLow(double theta, int dir, PolarWaveState &polar)  = 0;
    virtual int ThetaHi (double theta, int dir, PolarWaveState &polar)  = 0;
};

class PrandtlMeyer                               // abstract base class
{
/**********************************************************
Ref: Compressible Fluid Dynamics by P. A. Thompson
    de/dV       = -P(V,e)
    d(theta)/dV = (sqrt(M^2-1)/M^2)*rho
    where
      0.5*q^2 + e + P*V = const
      M = q/c >= 1
**********************************************************/
private:    
    PrandtlMeyer();                             // disallowed
    PrandtlMeyer(const PrandtlMeyer&);          // disallowed
    void operator=(const PrandtlMeyer&);        // disallowed
    PrandtlMeyer *operator &();                 // disallowed    
protected:
    PrandtlMeyer(EOS &e);
public:
    EOS *eos;
    PolarState state0;
    double c0;
    double P0;
    double B;                                   // B = 0.5*q^2 + e + P*V
public:
    virtual ~PrandtlMeyer();
    virtual int Initialize(PolarState &s);
    
    virtual int Theta(double theta, int dir, PolarWaveState &polar)     = 0;
    virtual int P(double p, int dir, PolarWaveState &polar)             = 0;
};

#endif // EOSLIB_SHOCK_POLAR_H
