#ifndef EOSLIB_HEfit_EOS_H
#define EOSLIB_HEfit_EOS_H

#include <ODE.h>
#include <OneDFunction.h>

#define HEfit_vers "HEfitv2.0.1"
#define HEfit_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *HEfitlib_vers;
extern const char *HEfitlib_date;

extern "C" { // DataBase functions
    char *EOS_HEfit_Init();
    char *EOS_HEfit_Info();
    void *EOS_HEfit_Constructor();
}


class HEfitdetonation;
class HEfit;

// Sam Shaw's fitting form for experimental data of detonation products
// ---------------------
// Release isentrope of overdrive plastic-bonded explosive PBX-9501
// R. S. Hixon, M. S. Shaw, J. N. Fritz, J. E. Vorthman, W. W. Anderson
// J. Appl. Phys. 88 (2000) pp 6287--6293

class HEfit : public EOS, private ODE
{
    friend class HEfitdetonation;
private:
	int F(double *y_prime, const double *y, double rho);  // ODE::F
    
	void operator=(const HEfit&);		// disallowed
	HEfit(const HEfit&);			    // disallowed
	HEfit *operator &();                // disallowed
protected:
// ambient reactant state
	double V0;
    double e0;
    double P0;
// CJ state relative to ambient reactant state
    double rho_CJ;
    double e_CJ;
    double P_CJ;
// bounds on domain
    double rho_min, rho_max;
    double Pmin, Pmax;
// approximate Gamma
    double Gamma0;
// polynomial fit to Pcj*rho^(-Gamma0)
    int nfCJ;
    double fCJ[10];
// polynomial fit to Gamma/V
    int ngCJ;
    double gCJ[10];
// x = rho - rho_CJ
    double sum_g(double x);       // sum_g(x) = sum(0,ngCJ) gCJ[i]*x^i
    double dsum_g(double x);      // dsum_g(x) = (d/dx) sum_g(x)        
    double sum_f(double x);       // sum_f(x) = sum(0,nfCJ) fCJ[i]*x^i
    double dsum_f(double x);      // dsum_f(x) = (d/dx) sum_f(x)
    
    double Pcj(double rho);       // Pcj(rho) = rho^Gamma0 * sum_f(rho-rho_CJ)
    double dPdrho(double rho);    // dPdrho(rho) = (d/d rho) Pcj(rho)
    double Ecj(double rho);       // Ecj(rho) = e_CJ + int Pcj(rho)*drho/rho^2

    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	HEfit();
	~HEfit();

// EOS functions
	double P(double V, double e); 	    // Pressure
	double c2(double V, double e);		// sound speed squared
	double Gamma(double V, double e);	// Gruneisen coefficient
 // double FD(double V, double e);		// Fundamental Derivative
	int NotInDomain(double V, double e);

// Not available
    double T(double V, double e);       // Temperature
	double S(double V, double e);	    // Entropy	
	double CV(double V, double e);		// Specific heat
	int PT(double P, double T, HydroState &state);
	Isotherm *isotherm(const HydroState &state);
// detonation allows non-equilibirum initial state for Hugoniot locus
    Detonation *detonation(const HydroState &state, double P0);
};

class HEfitdetonation : public Detonation, private OneDFunction
{
    enum Zero
    {
        pressure=0, velocity=1, wave_speed=2, CJ=3
    };
private:    
    int InitCJ();
    
    double V_min, V_max;                        // gas->rho_max & gas->rho_min
    double e_CJ;                                 // gas->e_CJ
    double f(double);                           // OneDFunction::f
    Zero fzero;                                 // flag for OneDFunction::f
    double Pav, m2, du2;                        // used in OneDFunction::f
    double P1, V1, e1, D;                       // set by OneDFunction::f
    
    HEfitdetonation(const HEfitdetonation&);    // disallowed
    void operator=(const HEfitdetonation&);     // disallowed
    HEfitdetonation *operator &();              // disallowed
public:
    double rel_tol;                             // tolerance for near CJ state
    HEfitdetonation(HEfit &gas, double pvac = EOS::NaN);
    ~HEfitdetonation();    

    int P  (double p,  int direction, WaveState &wave);
    int u  (double u,  int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    int w_u_s(double us, int direction, WaveState &wave);
    int V  (double V,  int direction, WaveState &wave);
};

#endif // EOSLIB_HEfit_EOS_H
