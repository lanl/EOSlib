#ifndef CCLIB_ODE_
#define CCLIB_ODE_

#include <cmath>
#include <List.h>

//    ERROR FLAGS
//        0 = OK (no error)
const int ODE_BUG = -99;
const int NOT_INIT = -3;
const int STEP_LIMIT = -2;
const int FAILED = -1;
const int NON_CONVERGENCE = 1;
const int STOP_CONDITION = 2;
const int STOP_ERROR = 3;
const int TIME_STEP_ERROR = 4;
const int EndList = 5;


class ODE;
class ODEfunc;
//
// private classes, only for internal use of ODE
//
class CacheState;
class ODEstate;
class ODEstop;
class ODEstopF;

//

class CacheState
{
    friend class ODE;
private:
    double t0;
    double h;               // t1 - t0

    double *y0;
    double *y0_prime;
        
    double *dy;             // y1 - y0
    double *c4;             // coeff. needed for 4th order interpolation
    
    double stop_condition;  // Value of StopCondition(y0,y0_prime,t0)
            
    CacheState();
    ~CacheState();
    void Initialize(int n);
    
    CacheState(const CacheState&);        // Disallowed, loader error
    void operator=(const CacheState&);    // Disallowed, loader error
public:
//  Quartic Hermite interpolation: y0, y0_prime, y_half, y1_0, y1_prime    
    void InitInterp(int n, double *dy_half, double *yp_1);    
    void Interpolate(int n, double *y1, double p, double *y,
                            double *yp=0) const;
    double T0() const { return t0; }
    double dt() const { return h; }
    double StopCondition() const { return stop_condition; }
    inline double EvalF(ODEfunc &cl) const;
};

class ODEstate
{
    friend class ODE;
    friend class LIST<ODEstate>;
private:
    double t0;
    double *y0;
    double *y0_prime;
        
    double h;                       // t1 - t0
    double stop_condition;          // Value of StopCondition(y0,y0_prime,t0)
    
    ODEstate();
    ODEstate(int n) { Initialize(n); }
    ~ODEstate();
    
    ODEstate(const ODEstate&);          // Disallowed, loader error
    void operator=(const ODEstate&);    // Disallowed, loader error
    
    void Initialize(int n);
    inline double EvalF(ODEfunc &cl) const;    
};

// Abstract base class ODE
//    derived class must supply
//    int F(double *y_prime, const double *y, double t)
//
class ODE
{
    friend class ODEstop;
    friend class ODEstopF;
private:
    ODE();                      // disallowed, need dimension of equations
    ODE(const ODE &);           // disallowed
    ODE& operator=(const ODE&); // disallowed

    const int n_dim;            // dimension of vector of equations
    int n_cache;                // size of inner cache, must be >= 2
    CacheState *Cache;          // array of inner cache states
    int cach0;                  // first cache entry
    int cach1;                  // last cache entry
    int cache;                  // most currently used cache point
    int dir;                    //  1 if t > t0
                                //  0 if cache empty
                                // -1 if t < t0
    int n_steps;                // number of steps since last outer cache point
    int init;                   // 1 if initialized
    ODEstate Init;              // initial state
    ODEstate InftyPlus, InftyMinus;
                // Auxiliary states at t = +infty and
                // t = -infty (there internal storage
                // for y0, y0_prime etc. is used elsewhere.    
    // Local arrays used in Step:
    double *dy_full,  *y_full;
    double *dy_half,  *y_half, *y_p_half;
    double *dy_2half, *y_2half;
    // Local arrays used in RungeKuttaStep:
    double *k1, *k2, *k3, *k4;
    //
    // Internal functions to manipulate vectors
    // 
    void AddVec(double *a, double *b, double *c);   // a = b + c
    void CopyVec(double *a, double *b);             // a = b
    void SwapVec(double *&a, double *&b);           // a <-> b
    void CopyState(ODEstate& a, ODEstate& b);       // a = b
    void CopyState(CacheState& a, ODEstate& b);     // a = b
    void CopyState(ODEstate& a, CacheState& b);     // a = b
    //
    // functions for internal use
    //
    int NextCache(int n) const { n++; return (n == n_cache) ? 0 : n; }
    int PrevCache(int n) const { n--; return (n < 0) ? n_cache-1 : n; }
    
    int FindNeighbor(double t);
    int FindNeighbor(ODEfunc &cl, double val);
    
    int ForwardBracket(double t);
    int BackwardBracket(double t);
    
    int Integrate(double t);
    int Integrate(ODEfunc &cl, double val);
    
    int ForwardIntegrate(double t);
    int BackwardIntegrate(double t);
    
    int Step(double t);
    int RungeKuttaStep(double* y0, double* y0_p, double* dy,
                        double t, double h);
    void RichardsonInterp(double *dy, double *dy_2half, double *dy_full);
protected:
    int NeverStop;              // True if stop condition is not calculated
    int IgnoreStop;             // True if stop condition is calculated 
                                //    for Cache but ignored
                
    double norm;                // last norm computed in Step()
    double t_last;              // last returned time

    LIST<ODEstate> Stops;       // List of zero's in the stop-condition
        // Stops always has 2 states; t0 = -HUGE_VAL and +HUGE_VAL
        // current link set such that t_last < Stops.Get()->t0
        // Note: IgnoreStopOn() sets Stops consistent with t_last      
    LIST<ODEstate> Initial;     // Outer Cache
        // List of saved intermediate points
        // Initial always has 2 states; t0 = -HUGE_VAL and +HUGE_VAL
    double t_min, t_max;        // limits on Initial

    void Interpolate(int cach, double t, double *y, double *yp = 0) const;
    int  Interpolate(ODEfunc &cl, double val, int cach, double& t,
                                  double *y, double *yp = 0);
    void Interp(const CacheState &C0, const CacheState &C1, double p,
                                  double *y,double *yp=0) const;
    void Interp(int *bracket, double p, double *y,double *yp=0) const;

    virtual double MaxNorm(const double *y_full, const double *y_2half,
                const double *dy_full, const double *dy_2half);
        // Returns estimate of error for pair of Runge-Kutta step
        // Default is maximum over components of
        //        (relative error) / epsilon
        // Step is accepted when MaxNorm < 1
        
    int Cach0() const { return cach0; }
    int Cach1() const { return cach1; }
    CacheState *CacheArray() const { return Cache; }
    
    ODE(int N_dim, int N_cache = 2);
        // N_dim = dimension of vector of equations is required
        // N_cache = size of inner cache >= 2 (optional)
    ~ODE();
public:
    double epsilon;         // relative accuracy,            default: 1.0e-10
    double dt_min;          // minimum allowed time step,    default: 1.0e-10
    double dt_max;          // maximum time step used,       default: HUGE_VAL
    int n_max;              // maximum number of time step,  default: 100
                            // divisions in one cycle
    int max_steps;          // Maximum number of steps,      default: 100000
                            // each call to Integrate()
    int CacheStep;          // Number of steps between outer cache states
                            // default: no cache,     if inner cache size = 2
                            //        inner cache size, otherwise
    // ODE integrates system of equations
    //    (d/dt) y = F(y,t)
    //        where y & F are vectors of length n_dim
    //    integration until requested t or StopCondition = 0
    //
    // Derived class MUST supply function F and set n_dim
    //
    int Initialize(double t, double *y_init, double dt);
        // Sets initial state t, y(t) or re-initializes
        // dt = estimate of first time step (not critical)
        // returns error status: 0 = OK, see above for errors
    int Integrate(double &t, double *y, double *yp = 0);
        // Integrates up to t or until StopCondition = 0
        // returns y and yp if non-zero pointer
        // returns error status: 0 = OK, STOP_CONDITION,
        //            and see above for errors
    int Integrate(ODEfunc &cl, double &val, double &t, double *y,
                            double *yp = 0);
        // Integrates up to cl.f(t, y, yp) = val or until StopCondition = 0
        //    f() must be monotonically increasing with t
        // returns y and yp if non-zero pointer
        // returns error status: 0 = OK, STOP_CONDITION,
        //            and see above for errors
    int Forward(double &t, double *y, double *yp = 0, int *bracket=0);
        // Integrates up to the first natural stop with t0 > t.
        // returns y and yp if non-zero pointer
        // returns error status: 0 = OK, or see above for errors
        // bracket if set returns two integers for the two
        // cache entries that bracket t.  
    int Backward(double &t, double *y, double *yp = 0, int *bracket=0);
        // Integrates down to the first natural stop with t0 < t.
        // returns y and yp if non-zero pointer
        // returns error status: 0 = OK, or see above for errors
        // bracket if set returns two integers for the two
        // cache entries that bracket t.  
    int GetOuterCacheState(int i, double &t, double *y, double *yp = 0);
        // returns t, y(t), yp(t)
        // for ith state from current location on outer cache list
        // i can be positive (latter t) of negative (earlier t)
        // return 0 = OK, EndList
    int GetStopState(int i, double &t, double *y = 0, double *yp = 0);
        // returns t, y(t), yp(t)
        // for ith state from current location on stop list
        // i can be positive (latter t) of negative (earlier t)
        // return 0 = OK, EndList
    virtual int F(double *y_prime, const double *y, double t) = 0;
        // Evaluates y_prime = F(y,t)
        // returns error status: 0 if sucessful, non-zero on ERROR
    virtual double StopCondition(const double *y, const double *yp,
                                double t);
        // Returns value of stop condition at t, y(t), yp(t)
        // WARNING: must NOT change either y or yp
        // Trivial default is provided, but if not needed,
        //    derived class constructor should set NeverStop = 1 
    virtual int StopTime(const CacheState &C0, const CacheState &C1,
                                double &t);
        // Given bracket and interpolation function,
        //     finds t such that StopCondition = 0
        // Default function is robust zero finder
        // returns error status: 0 = OK, non-zero = ERROR
    //
    //    fuctions provided for users convenience
    //
    int Dimension() const { return n_dim; }
    int InitialState(double &t, double *y, double *yp = 0);
        // t, y, yp set to initial state
        // returns 0 if initialized or 1 on error
    int LastState(double &t, double *y, double *yp = 0);
        // t, y, yp set to last calulated state on trajectory
        // returns dir
    double OuterCache_tmin() const { return t_min; }
    double OuterCache_tmax() const { return t_max; }
    void IgnoreStopOn()    { IgnoreStop = 1; }
    void IgnoreStopOff();    // Sets Stops and t_last consistent
    //
    //    diagnostic functions
    //
    int Ndim()   const {return n_dim;}      // dimension of vector of equations
    int Ncache() const {return n_cache;}    // size of inner cache
    double StepSize()      const { return dir ? Cache[cach1].h : Init.h; }
    double CacheStepSize() const { return Cache[cache].h; }
    int InnerCacheSize() const
    {
        int val = cach1 - cach0 + 1;
        return val > 0 ? val : n_cache + val;
    }
    int OuterCacheSize() const              { return Initial.Dimension()-2; }
    int SizeFromLastOuterCachePoint() const { return n_steps; }
    int StopListSize() const                { return Stops.Dimension()-2; }
    int Initialized() const                 { return init;}
    const char *ErrorStatus(int status) const;
};


// Abstract base class ODEfunc
//    derived class must supply
//    double f(double t, const double *y, const double *yp)
//
class ODEfunc
{
private:
    ODEfunc(const ODEfunc &);               // disallowed
    ODEfunc& operator=(const ODEfunc&);     // disallowed
public:
    double rel_tol;
    double abs_tol;
    ODEfunc() : rel_tol(1.e-8), abs_tol(1.e-14) {}
    ODEfunc(double rtol,double atol) : rel_tol(rtol), abs_tol(atol) {}
//  ~ODEfunc() {}
    virtual double f(double t, const double *y, const double *yp) = 0;
};
//
//    Inline functions
//
inline void ODE::AddVec(double *a, double *b, double *c)    // a = b + c
{
    int n = n_dim;
    while(n--)
        *a++ = *b++ + *c++;
}
inline void ODE::CopyVec(double *a, double *b)              // a = b
{
    int n = n_dim;
    while(n--)
        *a++ = *b++;
}
inline void ODE::SwapVec(double *&a, double *&b)            // a <-> b
{
    double *tmp = a;
    a = b;
    b = tmp;
}
inline void ODE::CopyState(ODEstate& a, ODEstate& b)        // a = b
{
    a.t0 = b.t0;
    a.h = b.h;
    a.stop_condition = b.stop_condition;
    CopyVec(a.y0, b.y0);
    CopyVec(a.y0_prime, b.y0_prime);
}
inline void ODE::CopyState(CacheState& a, ODEstate& b)      // a = b
{
    a.t0 = b.t0;
    a.h = b.h;
    a.stop_condition = b.stop_condition;
    CopyVec(a.y0, b.y0);
    CopyVec(a.y0_prime, b.y0_prime);
}
inline void ODE::CopyState(ODEstate& a, CacheState& b)      // a = b
{
    a.t0 = b.t0;
    a.h = b.h;
    a.stop_condition = b.stop_condition;
    CopyVec(a.y0, b.y0);
    CopyVec(a.y0_prime, b.y0_prime);
}
//
inline void ODE::Interpolate(int cach, double t, double *y, double *yp) const
{
    const CacheState &C0 = Cache[PrevCache(cach)];
    const CacheState &C1 = Cache[cach];
    C0.Interpolate(n_dim, C1.y0_prime, (t-C0.t0)/C0.h, y,yp);
}
inline void ODE::Interp(const CacheState &C0,const CacheState &C1, double p,
                        double *y,double *yp) const
{
    C0.Interpolate(n_dim, C1.y0_prime, p, y, yp);
}
inline void ODE::Interp(int *bracket, double p, double *y,double *yp) const
{
    Cache[bracket[0]].Interpolate(n_dim, Cache[bracket[1]].y0_prime, p, y, yp);
}
//
inline double CacheState::EvalF(ODEfunc &cl) const
{
    return cl.f(t0, y0, y0_prime);
}
//
inline double ODEstate::EvalF(ODEfunc &cl) const
{
    return (t0 == HUGE_VAL) ? HUGE_VAL :
            (t0 == -HUGE_VAL) ? -HUGE_VAL : cl.f(t0, y0, y0_prime);
}



#ifdef _USAGE_
/*******************************************************************************
// Example of usage:

class Harmonic : public ODE
{
    double Omega2;
    double beta;
public:
    Harmonic(double Omeg, double b) : ODE(2)
    {
        Omega2 = Omeg;
        beta = b;
    }
    
    int F(double *y_dot, const double *y, double t);
    double StopCondition(const double *y, const double *yp, double t);
};

int Harmonic::F(double *y_dot, const double *y, double)
{
//    damped Harmonic oscillator
//    (d/dt)^2 y + beta (d/dt) y + Omega2 y = 0
//    y[0] = y and y[1] = (d/dt) y

    y_dot[0] = y[1];
    y_dot[1] = -beta*y_dot[0] - Omega2*y[0];
    
    return 0;
}

// Stop when total energy = 0.5
double Harmonic::StopCondition(const double *y, const double *, double)
{
    return Omega2*y[0]*y[0] + y[1]*y[1] - 0.5;
}

*******************************************************************************/
#endif /* _USAGE_ */

#endif /* CCLIB_ODE_ */
