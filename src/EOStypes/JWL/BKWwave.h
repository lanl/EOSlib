#ifndef EOSLIB_BKW_WAVE_H
#define EOSLIB_BKW_WAVE_H

#include <BKW.h>
#include <OneDFunction.h>

class BKWdetonation : public Detonation, private OneDFunction
{
    enum Zero
    {
        pressure=0, velocity=1, wave_speed=2, CJ=3
    };
private:    
    int InitCJ();
    
    double Vlimit;                              // limiting V on shock locus
    double f(double);                           // OneDFunction::f
    Zero fzero;                                 // flag for OneDFunction::f
    double Pav, m2, du2;                        // used in OneDFunction::f
    double P1, V1, e1, D;                       // set by OneDFunction::f
    
    BKWdetonation(const BKWdetonation&);        // disallowed
    void operator=(const BKWdetonation&);       // disallowed
    BKWdetonation *operator &();                // disallowed
public:
    double rel_tol;                             // tolerance for near CJ state
    BKWdetonation(BKW &gas, double pvac = EOS::NaN);
    ~BKWdetonation();    

    int P  (double p,  int direction, WaveState &wave);
    int u  (double u,  int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    int w_u_s(double us, int direction, WaveState &wave);
    int V  (double V,  int direction, WaveState &wave);
};

class BKWdeflagration : public Deflagration, private OneDFunction
{
    enum Zero
    {
        pressure=0, velocity=1, wave_speed=2, CJ=3, VMIN=4
    };
private:    
    int InitCJ();
    
    double f(double);                           // OneDFunction::f
    Zero fzero;                                 // flag for OneDFunction::f
    double Pav, m2, du2;                        // used in OneDFunction::f
    double P1, V1, e1, D;                       // set by OneDFunction::f
    
    BKWdeflagration(const BKWdeflagration&);    // disallowed
    void operator=(const BKWdeflagration&);     // disallowed
    BKWdeflagration *operator &();              // disallowed
public:
    double rel_tol;                             // tolerance for near CJ state
    BKWdeflagration(BKW &gas, double pvac = EOS::NaN);
    ~BKWdeflagration();    

    int P  (double p,  int direction, WaveState &wave);
    int u  (double u,  int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    int V  (double V,  int direction, WaveState &wave);
};


#endif // EOSLIB_BKW_WAVE_H

