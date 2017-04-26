#include <iostream>
#include <iomanip>
using namespace std;
#include "EOS.h"

ostream &operator <<(ostream &out, const Format &f)
{
/*****
    if( f.fixed )
        out.setf(ios::fixed, ios::floatfield);
    else
        out.setf(ios::scientific, ios::floatfield);
    
    out << setw(f.width) << setprecision(f.precision);
    return out;
*****/
    return out << (f.fixed ? fixed : scientific)
               << setw(f.width) << setprecision(f.precision);
}

ostream &operator<<(ostream &out, const Center &c)
{
    int len = strlen(c.str);
    int w_left = (1+c.width - len)/2;
    int w_right = c.width - len - w_left;
    if( w_left > 0 ) out << setw(w_left) << " ";
    out << c.str;
    if(w_right > 0 ) out << setw(w_right) << " ";
    
    return out;
}

Format *_V_form  = NULL;
Format *_e_form  = NULL;
Format *_u_form  = NULL;
Format *_P_form  = NULL;
Format *_T_form  = NULL;
Format *_c_form  = NULL;
Format *_us_form = NULL;
Format *_q_form     = NULL;
Format *_theta_form = NULL;

//

void InitFormat()
{
    _V_form  = new Format;
    _e_form  = new Format;
    _u_form  = new Format;
    _P_form  = new Format;
    _T_form  = new Format;
    _c_form  = new Format;
    _us_form = new Format;
    _q_form     = new Format;
    _theta_form = new Format;
}

ostream & operator<<(ostream &out, const HydroState &state)
{
    return out << V_form << state.V << " "
           << e_form << state.e << " "
           << u_form << state.u;
}
ostream &HydroStateLabel(ostream &out)
{
    return out << Center(V_form,"V") << " "
               << Center(e_form,"e") << " "
               << Center(u_form,"u");
}
ostream &HydroStateLabel(ostream &out, const Units &u)
{
    return out << Center(V_form,u.Unit("specific_volume")) << " "
               << Center(e_form,u.Unit("specific_energy")) << " "
               << Center(u_form,u.Unit("velocity"));
}

//

ostream & operator<<(ostream &out, const ThermalState &state)
{
    return out << V_form << state.V << " "
           << e_form << state.e << " "
           << P_form << state.P << " "
           << T_form << state.T;
}
ostream &ThermalStateLabel(ostream &out)
{
    return out << Center(V_form,"V") << " "
               << Center(e_form,"e") << " "
               << Center(P_form,"P") << " "
               << Center(T_form,"T");
}
ostream &ThermalStateLabel(ostream &out, const Units &u)
{
    return out << Center(V_form,u.Unit("specific_volume")) << " "
               << Center(e_form,u.Unit("specific_energy")) << " "
               << Center(P_form,u.Unit("pressure")) << " "
               << Center(T_form,u.Unit("temperature"));
}

//

ostream & operator<<(ostream &out, const HydroThermal &state)
{
    return out << (HydroState &)state << " "
               << (ThermalVariables &)state;
}
ostream &HydroThermalLabel(ostream &out)
{
    HydroStateLabel(out) << " ";
    return ThermalVariablesLabel(out);
}
ostream &HydroThermalLabel(ostream &out, const Units &u)
{
    HydroStateLabel(out, u) << " ";
    return ThermalVariablesLabel(out, u);
}

//

ostream & operator<<(ostream &out, const ThermalVariables &state)
{
    return out << P_form << state.P << " "
           << T_form << state.T << " "
           << c_form << state.c;
}
ostream &ThermalVariablesLabel(ostream &out)
{
    return out << Center(P_form,"P") << " "
               << Center(T_form,"T") << " "
               << Center(c_form,"c");
}
ostream &ThermalVariablesLabel(ostream &out, const Units &u)
{
    return out << Center(P_form,u.Unit("pressure")) << " "
               << Center(T_form,u.Unit("temperature")) << " "
               << Center(c_form,u.Unit("velocity"));
}

//


ostream & operator<<(ostream &out, const WaveState &state)
{
    return out << (HydroState &)state << " "
               << P_form << state.P   << " "
               << us_form << state.us;
}
ostream &WaveStateLabel(ostream &out)
{
    return HydroStateLabel(out)  << " "
           << Center(P_form,"P") << " "
           << Center(us_form,"us");
}
ostream &WaveStateLabel(ostream &out, const Units &u)
{
    return HydroStateLabel(out,u) << " "
           << Center(P_form,u.Unit("pressure")) << " "
           << Center(us_form,u.Unit("velocity"));
}

//

ostream & operator<<(ostream &out, const PolarState &state)
{
    return out << V_form << state.V << " "
           << e_form     << state.e << " "
           << q_form     << state.q << " "
           << theta_form << state.theta;
}
ostream &PolarStateLabel(ostream &out)
{
    return out << Center(V_form,"V") << " "
               << Center(e_form,"e") << " "
               << Center(q_form,"q") << " "
               << Center(theta_form,"theta");
}
ostream &PolarStateLabel(ostream &out, const Units &u)
{
    return out << Center(V_form,u.Unit("specific_volume")) << " "
               << Center(e_form,u.Unit("specific_energy")) << " "
               << Center(q_form,u.Unit("velocity")) << " "
               << Center(theta_form,"radian");
}

//


ostream & operator<<(ostream &out, const PolarWaveState &state)
{
    return out << (PolarState &)state << " "
               << P_form << state.P   << " "
               << theta_form << state.beta;
}
ostream &PolarWaveStateLabel(ostream &out)
{
    return PolarStateLabel(out)  << " "
           << Center(P_form,"P") << " "
           << Center(theta_form,"beta");
}
ostream &PolarWaveStateLabel(ostream &out, const Units &u)
{
    return PolarStateLabel(out,u) << " "
           << Center(P_form,u.Unit("pressure")) << " "
           << Center(theta_form,"radian");
}
