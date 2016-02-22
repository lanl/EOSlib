#include "LocalMath.h"


numeric_limits<double> Dlimits;
const double NaN = Dlimits.quiet_NaN();
//const double NaN = numeric_limits<double>::quiet_NaN();

// Check if the following is still needed ?
/********************
inline double Div(double x, double y) { return x/y; }
//double NaN = Div(0., 0.);
//
// kludge to initialize externs for main program linked to shared object
// 
int NaN_set = 0;
int NaN_Init()
{
    if( NaN_set == 0 )
    {
        NaN = Div(0.,0.);
        NaN_set = 1;
    }
    return 1;
}
****************/
