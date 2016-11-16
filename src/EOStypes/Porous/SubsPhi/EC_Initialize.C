#include "PhiTypes.h"
#include "ECfit.h"
#include <EOS.h>

/*********************************************************
*    Compare configuration pressure with
*    quasi-static compaction with experiment
*    of Elban & Chiarito
*        Powder Technology 46 (1986) pp. 181--193
**********************************************************/

#define FUNC "EC::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int EC::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
// Should go outside subroutine
// But not getting loaded with shared object on Linux

static double A_11[10] = {   11.18,        // MPa
              32.39,
              42.47,
              57.55,
             183.8,
             136.6,
            -378.3,
            -307.1,
             450.4,
             396.7
          };          
static ECfit HMX11(A_11, 0.646, 0.96);

static double A_17[10] = {    9.051,        // MPa
              24.68,
              45.05,
              66.30,
              51.92,
              -4.105,
              21.76,
             134.2,
             134.1,
              40.98
          };          
static ECfit HMX17(A_17, 0.605, 0.962);

static double A_8[10] = {     8.739,        // MPa
              21.61,
              34.84,
              61.02,
              98.66,
              40.63,
             -96.42,
             -10.96,
             185.9,
             120.1
          };          
static ECfit HMX8(A_8, 0.563, 0.956);

static double A_9[10] = {     9.726,        // MPa
              24.86,
              40.40,
              91.98,
             172.6,
              16.61,
            -266.9,
              21.99,
             468.0,
             269.4
          };          
static ECfit HMX9(A_9, 0.559, 0.947);

//
    delete [] expt;
    delete [] hmx;
    delete data;
    expt = hmx = NULL;
    data = NULL;
    calc.Variable("expt", &expt);
    calc.Variable("hmx",  &hmx);
            
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( expt == NULL || hmx == NULL )
    {
        EOSerror->Log(FUNC, "variable not set: expt=%s, hmx=%s\n", expt, hmx );
        return 1;    
    }


    ECfit *ECdata = NULL;
    int nsteps = 1000;
    
    if( !strcmp(expt,"HMX11") )
        ECdata = &HMX11;
    else if( !strcmp(expt,"HMX17") )
        ECdata = &HMX17;
    else if( !strcmp(expt,"HMX8") )
        ECdata = &HMX8;
    else if( !strcmp(expt,"HMX9") )
        ECdata = &HMX9;
    else
    {
        EOSerror->Log(FUNC, "unknown expt `%s'\n", expt );
        return 1;    
    }

 // EOS needed to convert experimental TMD into true volume fraction
    EOS *eos = FetchEOS(hmx,*db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS failed for hmx = %s\n", hmx );
        return 1;    
    }

 // Assume inital state is reference state
    if( std::isnan(eos->V_ref) || std::isnan(eos->e_ref) )
    {
        EOSerror->Log(FUNC, "ref state not defined for hmx = %s\n", hmx );
        return 1;    
    }
    HydroState Hstate(eos->V_ref, eos->e_ref);
    
 // Assume experimental data is isothermal
    Isotherm *isoT = eos->isotherm(Hstate);
    if( isoT == NULL )
    {
        EOSerror->Log(FUNC, "eos->isotherm failed for hmx = %s\n", hmx );
        return 1;    
    }
        
    ThermalState Tstate;
    if( isoT->P(ECdata->beta_1,Tstate) )
    {
        EOSerror->Log(FUNC, "isoT->P failed for %s\n", hmx );
        return 1;    
    }
    
    double *PV  = new double[nsteps+1];
    double *phi = new double[nsteps+1];
    
    double Vs_0 = eos->V_ref;
    double dV = (Vs_0-Tstate.V)/nsteps;
    int i;
    double Vs = Vs_0;
    for( i=0; i<nsteps+1; i++, Vs -= dV )
    {
     // TMD into true volume fraction    
        if( isoT->V(Vs,Tstate) )
        {
            EOSerror->Log(FUNC, "isoT->V failed for %s\n", hmx );
            return 1;    
        }            
        double beta = Vs*Tstate.P/Vs_0;
        double TMD = ECdata->TMD(beta);
        
        phi[i] = TMD*(Vs/Vs_0);
        PV[i] = Tstate.P*Vs;
    }
    Pmin = eos->P(Hstate);
    delete isoT;
    deleteEOS(eos);
 //     
    data = new Spline(nsteps+1, &PV, &phi, NaN,NaN);    
    int N = data->Nknot();
    if( InitPV(data->x(0), data->x(N-1)) )
    {
        EOSerror->Log(FUNC, "InitPV(PV_0,PV_1) failed\n" );
        return 1;
    }
    
    return 0;
}
