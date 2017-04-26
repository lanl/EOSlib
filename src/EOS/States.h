#ifndef EOSLIB_STATES_H
#define EOSLIB_STATES_H

class ThermalState;
class HydroState;
class ThermalVariables;
class HydroThermal;
class WaveState;
class PolarState;
class PolarWaveState;

//

class ThermalState
{
public:
    double V;
    double e;
    
    double P;
    double T;

    ThermalState() {}
    ThermalState(double vp, double ep, double p, double t)
                : V(vp), e(ep), P(p), T(t) {}
};

class HydroState
{
public:
    double V;
    double e;
    double u;
    
    HydroState() {}
    HydroState(double vp, double ep, double up = 0.) : V(vp), e(ep), u(up) {}
};

class ThermalVariables
{
public:
    double P;
    double T;
    double c;
};

class HydroThermal : public HydroState, public ThermalVariables
{
};


class WaveState : public HydroState
{
public:
    double P;
    double us;
};


//

class PolarState
{
public:
    double V;
    double e;
    double q;       // magnitude of particle velocity
    double theta;   // angle of particle velocity relative to x-axis
                    // -pi < theta <= pi
    PolarState() {}
    PolarState(double vp, double ep, double qp, double thetap = 0.)
        : V(vp), e(ep), q(qp), theta(thetap) {}    
};

class PolarWaveState : public PolarState
{
public:
    double P;
    double beta;    // -pi/2 < beta < pi/2
                    // angle between shock front and x-axis
                    // or for Prandtl-Meyer fan, characteristic and x-axis
    //double us;
    //double c;
};


#endif // EOSLIB_STATES_H
