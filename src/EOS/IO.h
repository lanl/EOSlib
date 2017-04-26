#ifndef EOSLIB_IO_H
#define EOSLIB_IO_H

#include <iosfwd>
using std::ostream;

class Format
{
public:
    int width;
    int precision;
    int fixed;
    Format() : width(11), precision(4), fixed(0) {}
    Format(int w , int p, int f=0) : width(w), precision(p), fixed(f) {}
};
ostream &operator<<(ostream &out, const Format &f);

class Center
{
public:
    int width;
    const char *str;
    Center(const Format &f, const char *s) : width(f.width), str(s) {}
    Center(const char *s, const Format &f) : width(f.width), str(s) {}
};
ostream &operator<<(ostream &out, const Center &c);

extern Format *_V_form;
extern Format *_e_form;
extern Format *_u_form;
extern Format *_P_form;
extern Format *_T_form;
extern Format *_c_form;
extern Format *_us_form;
extern Format *_q_form;
extern Format *_theta_form;

#define V_form  (*_V_form)
#define e_form  (*_e_form)
#define u_form  (*_u_form)
#define P_form  (*_P_form)
#define T_form  (*_T_form)
#define c_form  (*_c_form)
#define us_form (*_us_form)
#define q_form      (*_q_form)
#define theta_form  (*_theta_form)

void InitFormat();


ostream &HydroStateLabel(ostream &out);
ostream &HydroStateLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const HydroState &state);


ostream &ThermalStateLabel(ostream &out);
ostream &ThermalStateLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const ThermalState &state);

ostream &HydroThermalLabel(ostream &out);
ostream &HydroThermalLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const HydroThermal &state);

ostream &ThermalVariablesLabel(ostream &out);
ostream &ThermalVariablesLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const ThermalVariables &state);

ostream &WaveStateLabel(ostream &out);
ostream &WaveStateLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const WaveState &state);

ostream &PolarStateLabel(ostream &out);
ostream &PolarStateLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const PolarState &state);

ostream &PolarWaveStateLabel(ostream &out);
ostream &PolarWaveStateLabel(ostream &out, const Units &units);
ostream & operator<<(ostream &out, const PolarWaveState &state);

#endif // EOSLIB_IO_H
