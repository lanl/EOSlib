#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"
#include <ODE.h>

class EVT : public ODE
{
private:
    EVT();                      // disallowed
    EVT(const EVT&);            // disallowed
    void operator=(const EVT&); // disallowed
    double y[1];
    double yp[1];
public:
    EOS *eos;
    ThermalState state0;
    EVT(EOS &e);
    ~EVT();    
    int Initialize(double V, double e);
    int F(double *yp, const double *y, double e);

    int T(double T1, ThermalState &state);
};
EVT::EVT(EOS &e) : ODE(1,512)
{
    eos = e.Duplicate();
}
EVT::~EVT()
{
    deleteEOS(eos);
}
int EVT::Initialize(double V, double e)
{
    state0.V = V;
    state0.e = e;
    state0.P = eos->P(V,e);
    state0.T = eos->T(V,e);
    double Cv = eos->CV(V,e);
    double de = 0.001*e + +0.1*state0.T*Cv;
    y[0] = e;
    int status = ODE::Initialize(state0.T, y, de);
    if( status )
        cerr << Error("ODE::Initialize failed with status ")
             << ODE::ErrorStatus(status)
             << Exit;
    return status;
}
int EVT::F(double *yp, const double *y, double)
{
    double e = y[0];
    yp[0] = eos->CV(state0.V,e);
    return std::isnan(yp[0]);    
}
int EVT::T(double T1, ThermalState &state)
{
    int status = Integrate(T1, y, yp);
    if( status )
        cerr << Error("ODE::Integrate failed with status ")
             << ODE::ErrorStatus(status)
             << Exit;
    state.V = state0.V;
    state.e = y[0];
    state.P = eos->P(state.V,state.e);
    state.T = eos->T(state.V,state.e);
    return 0;
}


int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.BM";
    const char *units = NULL;
    const char *lib   = "../../lib/Linux";
    int nsteps  = 10;
    double V = 0.;
    double e = 0.;
    double T_min = 300.;
    double T_max = 3000.;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(lib,lib);
        //
        GetVar(nsteps,nsteps);
        GetVar(V,V);
        GetVar(e,e);
        GetVar(Tmin,T_min);
        GetVar(Tmax,T_max);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
#ifdef LINUX
    setenv("SharedLibDirEOS",lib,1);
#else
    putenv(Cat("SharedLibDirEOS=",lib));
#endif
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;

    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    PTequilibrium *PT = HE->HE;
    EOS *reactants = PT->eos1;
    EOS *products  = PT->eos2;
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double P0 = reactants->P(Vref,eref);
    if( V <= 0. )
        V = Vref;
    if( e <= 0. )
        e = eref;
    EVT Treactants(*reactants);
    if( Treactants.Initialize(V,e) )
        cerr << Error("Treactants.Initialize failed") << Exit;
    EVT Tproducts(*products);
    if( Tproducts.Initialize(V,e) )
        cerr << Error("Tproducts.Initialize failed") << Exit;
	//
    cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# T"
         << " " << setw(13) << "   P_r"
         << " " << setw(13) << "   Cv_r"
         << " " << setw(13) << "   P_p"
         << " " << setw(13) << "   Cv_p"
         << "\n";    
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    //
    int i;
    ThermalState state_r;
    ThermalState state_p;
    for( i=0; i<=nsteps; i++ )
    {
        double T = T_min + double(i)/double(nsteps) * (T_max-T_min);
        if( Treactants.T(T,state_r) )
            cerr << Error("Treactants.T failed") << Exit;
        if( Tproducts.T(T,state_p) )
            cerr << Error("Tproducts.T failed") << Exit;
        double Cvr = reactants->CV(V,state_r.e);
        double Cvp = products->CV(V,state_p.e);
        cout        << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << state_r.P
             << " " << setw(13) << setprecision(6) << Cvr
             << " " << setw(13) << setprecision(6) << state_p.P
             << " " << setw(13) << setprecision(6) << Cvp
             << "\n";
    }
    deleteEOS(reactants);
    deleteEOS(products);
    return 0;
}
