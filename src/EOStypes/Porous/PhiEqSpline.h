#ifndef EOSLIB_PHI_EQ_SPLINE_H
#define EOSLIB_PHI_EQ_SPLINE_H

#include <LocalMath.h>
#include <ODE.h>
#include <OneDFunction.h>
#include <Spline.h>

#include <PhiEq.h>

//

class PhiEq_energy;
class PhiEq_spline;

//

class PhiEq_energy : public SplineFunction
{
public:
    PhiEq_spline *phi_eq;
    
    PhiEq_energy(PhiEq_spline *phi) : phi_eq(phi) { }
    double f(double phi);                       // SplineFunction.f
};


class PhiEq_spline : public PhiEq,              // Extended abstract base class
                     protected SplineFunction, protected ODE
{
// SplineFunction is for Phi, virtual function f
// ODE is for energy, function F
public:
    SplineFunction::abs_tol;
    SplineFunction::rel_tol;
private:
    void operator=(const PhiEq_spline&);        // disallowed
    PhiEq_spline(const PhiEq_spline&);          // disallowed
    PhiEq_spline *operator &();                 // disallowed, use Duplicate()
protected:
    double PV_smax;                             // maximum for SplineFunction
    double phi_smax;                            // maximum for SplineFunction
    double m;                                   // power for phi extrapolation
    PhiEq_energy Espline;
    double s_e;                                 // scale factor for units

    PhiEq_spline(const char *n="PhiEq_spline");
  //virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
public:
    virtual int InitPV(double PV_0, double PV_1);
    virtual ~PhiEq_spline();
    
    double fp(double PV);
    virtual double f(double PV) = 0;                    // SplineFunction.f
    int F(double *yp, const double *y, double phi);     // ODE.F

 // defaults use extrapolate beyond inteval from InitPV()
    virtual double phi(double PV);                      // from spline
    virtual double PV(double phi);                      // from spline
    virtual double energy(double phi);                  // from spline
    virtual double Energy(double phi);                  // from ODE
    
    virtual int    phi(double PV,  double phi[3]);      // from spline
    virtual int energy(double phi, double e[3]);        // from spline
};

#endif    // EOSLIB_PHI_EQ_SPLINE_H
