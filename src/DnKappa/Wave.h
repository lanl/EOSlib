#ifndef EOSLIB_WAVE_H
#define EOSLIB_WAVE_H

class Wave;        // common part of Isentrope and Hugoniot
class Isentrope;
class Hugoniot;
class Isotherm;

class EOS;        // EOS.h

// Wave direction
const int LEFT  = -1;
const int RIGHT =  1;
//
class Wave  // abstract base class
{           // common part of Isentrope, Hugoniot, Detonation, Deflagration
private:
    Wave();                                 // disallowed
    Wave(const Wave&);                      // disallowed
    void operator=(const Wave&);            // disallowed
    Wave *operator &();                     // disallowed
protected:
    EOS *eos;
    HydroThermal state0;                    // initial state       
    Wave(EOS &e, double pvac);
public:
    double p_vac;                           // default: eos->P_vac
    
    virtual ~Wave();    
    virtual int Initialize(const HydroState &state);
    // functions return error status
    //  -1 error, wave not set
    //   0 OK
    //   1 error, wave set but not at requested value of variable
    virtual int   V(double V,  int direction, WaveState &wave) = 0;
    virtual int   P(double p,  int direction, WaveState &wave) = 0;
    virtual int   u(double up, int direction, WaveState &wave) = 0;
    virtual int u_s(double us, int direction, WaveState &wave) = 0;
    // weak detonation: default return 1
    virtual int w_u_s(double us, int direction, WaveState &shock);    
    
    EOS &Eos()                         {return *eos;}
    const HydroThermal &State0() const {return state0;}
};

class Isentrope : public Wave               // abstract base class
{
private:    
    Isentrope();                            // disallowed
    Isentrope(const Isentrope&);            // disallowed
    void operator=(const Isentrope&);       // disallowed
    Isentrope *operator &();                // disallowed
protected:
    Isentrope(EOS &e, double pvac) : Wave(e, pvac) {}
public:
    virtual ~Isentrope();    
    virtual int Initialize(const HydroState &state);
    // find state on isentrope at specified V, P or u, u_s
    // functions P,V,u,u_s return error status
    //  -1 error, wave not set
    //   0 OK
    //   1 error, wave set to P = p_vac
    virtual int   V(double V,  int direction, WaveState &wave) = 0;
    virtual int   P(double p,  int direction, WaveState &wave) = 0;
    virtual int   u(double up, int direction, WaveState &wave) = 0;
    virtual int u_s(double us, int direction, WaveState &wave) = 0;
    // escape velocity corresponds to p = p_vac
    virtual double u_escape(int direction);
    virtual int    u_escape(int direction, WaveState &wave);
};
//
// class Hugoniot for equilibrium initial state
//      P0 = eos.P(V0,e0)
// class Detonation for non-equilibrium exothermic initial state
//      P0 < eos.P(V0,e0), compresive wave V < V0
// class Deflagration for non-equilibrium exothermic initial state
//      P0 < eos.P(V0,e0), expansive wave  V > V0
//
class Hugoniot : public Wave                // abstract base class
{
private:    
    Hugoniot();                             // disallowed
    Hugoniot(const Hugoniot&);              // disallowed
    void operator=(const Hugoniot&);        // disallowed
    Hugoniot *operator &();                 // disallowed
protected:
    Hugoniot(EOS &e, double pvac) : Wave(e,pvac) { }
public:
    virtual ~Hugoniot();    
    virtual int Initialize(const HydroState &state);

    // requires EOS satisfy weak condition for uniqueness
    virtual int P(double p, int direction, WaveState &shock)    = 0;
    // requires EOS satisfy medium condition for uniqueness
    virtual int u(double u, int direction, WaveState &shock)    = 0;
    // requires Convex EOS for uniqueness
    virtual int u_s(double us, int direction, WaveState &shock) = 0;
    // requires EOS satisfy strong condition for uniqueness
    // non-existence if V_0/V > maximum compression ratio
    virtual int V(double V,  int direction, WaveState &wave)    = 0;
};

class Detonation : public Wave              // abstract base class
{
private:    
    Detonation();                           // disallowed
    Detonation(const Detonation&);          // disallowed
    void operator=(const Detonation&);      // disallowed
    Detonation *operator &();               // disallowed
protected:
    double P0, V0, e0, u0;                  // initial state, P0 < P(V0,e0)
    double Pcj, Dcj, Vcj, ucj, ecj;         // CJ wave
    double Pw;                              // P(V0,e0), end of weak branch
    
    Detonation(EOS &gas, double pvac);   
    virtual int InitCJ() = 0;           // Called by Initialize to set CJ state
public:
    virtual ~Detonation();
    int Initialize(const HydroState &);     // return error
    int Initialize(const HydroState &state, double p0);
    int InitState(HydroState &state, double &p0);
    int CJwave(int direction, WaveState &wave);
    // strong branch of detonation locus
    virtual int P  (double p,  int direction, WaveState &wave) = 0;
    virtual int u  (double u,  int direction, WaveState &wave) = 0;
    virtual int u_s(double us, int direction, WaveState &wave) = 0;
    virtual int V  (double V,  int direction, WaveState &wave) = 0;
    // weak branch of detonation locus
    virtual int w_u_s(double us, int direction, WaveState &wave) = 0;
};

class Deflagration : public Wave            // abstract base class
{
private:    
    Deflagration();                         // disallowed
    Deflagration(const Deflagration&);      // disallowed
    void operator=(const Deflagration&);    // disallowed
    Deflagration *operator &();             // disallowed
protected:
    double P0, V0, e0, u0;                  // initial state, P0 < P(V0,e0)
    double Vp0, ep0;                        // wave at P=P0
    double Pcj, Dcj, Vcj, ucj, ecj;         // CJ wave
    
    Deflagration(EOS &gas, double pvac);   
    virtual int InitCJ() = 0;           // Called by Initialize to set CJ state
public:
    virtual ~Deflagration();
    int Initialize(const HydroState &);     // return error
    int Initialize(const HydroState &state, double p0);
    int InitState(HydroState &state, double &p0);
    int CJwave(int direction, WaveState &wave);
    int P0wave(int direction, WaveState &wave);
    // weak branch of deflagration locus
    virtual int P  (double p,  int direction, WaveState &wave) = 0;
    virtual int u  (double u,  int direction, WaveState &wave) = 0;
    virtual int u_s(double us, int direction, WaveState &wave) = 0;
    virtual int V  (double V,  int direction, WaveState &wave) = 0;
};

class Isotherm                              // abstract base class
{
private:
    Isotherm();                             // disallowed
    Isotherm(const Isotherm&);              // disallowed
    void operator=(const Isotherm&);        // disallowed
    Isotherm *operator &();                 // disallowed
protected:
    EOS *eos;
    ThermalState state0;                    // initial state
    Isotherm(EOS &e,double pvac);
public:
    virtual ~Isotherm();    
    virtual int Initialize(const HydroState &state);

    double p_vac;                           // default: eos->P_vac
    // functions P,V return error status
    //  -1 error, state not set
    //   0 OK
    //   1 error, state set to P = p_vac
    virtual int P(double p, ThermalState &state) = 0;
    virtual int V(double v, ThermalState &state) = 0;
    
    EOS &Eos()                         {return *eos;}
    const ThermalState &State0() const {return state0;}
};

#endif // EOSLIB_WAVE_H
