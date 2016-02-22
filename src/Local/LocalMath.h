#ifndef CCLIB_LOCAL_MATH_
#define CCLIB_LOCAL_MATH_

#ifdef AMRITA
#include <AMRdefines.h>
#endif

#include <cstdlib>
#include <cmath>
using std::abs;

#include <limits>
using std::numeric_limits;
extern numeric_limits<double> Dlimits;
extern const double NaN;


#ifdef PI
#undef PI
#endif
#define PI 3.1415926535897932

inline double sqr(double x) { return x*x; }


inline void sincos(double x, double *s, double *c)
{
	*s = sin(x);
	*c = cos(x);
}

inline double atan2pi(double imag, double real)
{
	return atan2(imag, real)/PI;
}

inline void sincospi(double x, double *s, double *c)
{
	*s = sin(x*PI);
	*c = cos(x*PI);
}


/*********************
#ifndef _NaN_
#define _NaN_
// kludge to initialize externs for main program linked to shared object
extern int NaN_set;
int NaN_Init();
static int initNaN_so = NaN_Init();
#endif
**************/


/**********************
#include <algorithm>
using std::min;
using std::max;
#include <utility>
using std::swap; // C++10, previously <algorithm>
**********************/

#ifndef _MIN_MAX_FUNCTION
 inline double min(double x, double y) {return x > y ? y : x ;}
 inline int min(int x, int y) {return x > y ? y : x ;}

 inline double max(double x, double y) {return x < y ? y : x ;}
 inline int max(int x, int y) {return x < y ? y : x ;}
#endif

#ifndef _SIGN_FUNCTION
 inline int sign(double x) {return x < 0 ? -1 : x > 0 ? 1 : 0;}
 inline int sign(int x) {return x < 0 ? -1 : x > 0 ? 1 : 0;}
#endif


/*************************************************************

#define _ABS_DOUBLE
#define _ABS_INT
#define _ROUND

#ifndef _ABS_INT
 inline int abs(int x)		{return x >= 0 ? x : -x;}
#endif

#ifndef _ABS_DOUBLE
 inline double abs(double x)	{return x >= 0 ? x : -x;}
#endif

#ifndef _ROUND
  inline int round(double x) {return x >= 0 ? (int)(x + 0.5) : (int)(x-0.5);}
#endif

*************************************************************/


#endif	/* CCLIB_LOCAL_MATH_ */
