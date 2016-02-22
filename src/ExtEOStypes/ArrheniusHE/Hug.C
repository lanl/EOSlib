#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file  = "Test.data";
    const char *type  = "ArrheniusHE";
    const char *name  = "PBX9501.sam";
    const char *units = NULL;
    int nsteps  = 10;
    
    EOS::Init();
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(units,units);
        GetVar(nsteps,nsteps);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ArrheniusHE *HE = dynamic_cast<ArrheniusHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *reactants = HE->reactants();
    EOS *products  = HE->products();
    double lambda_ref = HE->lambda_ref();
    double z[1] = {lambda_ref};
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    WaveState CJ;
    HydroState state0(Vref,eref);
    Detonation *det = HE->detonation(state0,Pref);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    Hugoniot *hug = reactants->shock(state0);
    if( hug == NULL )
        cerr << Error("eos1->shock failed") << Exit;
    WaveState VN;
    if( hug->u_s(CJ.us,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    //
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(9) << " Pshock"
         << " " << setw(9) << " Dshock "
         << " " << setw(9) << " Tshock "
         << " " << setw(9) << " Pdef"
         << " " << setw(9) << " Ddef"
         << " " << setw(9) << " Tdef"
         << " " << setw(9) << " u+c_def"
         << " " << setw(9) << " Pcv"
         << " " << setw(9) << " Tcv"
         << " " << setw(9) << " ccv"
         << " " << setw(9) << " u+c"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    int i;
    double P;
    for( i=0; i<=nsteps; i++ )
    {
        double P = CJ.P + double(i)*(VN.P-CJ.P)/double(nsteps);
        WaveState shock;
        if( hug->P(P,RIGHT,shock) )
            cerr << Error("hug->P failed") << Exit;
        HydroState state1 = shock;
        Deflagration *def = HE->deflagration(state1,P);
        if( def == NULL )
            cerr << Error("HE->deflagration failed" ) << Exit;
        WaveState CJdef;
        if( def->CJwave(RIGHT,CJdef) )
            cerr << Error("def->CJwave failed" ) << Exit;
        double Tshock = reactants->T(shock.V,shock.e);
        double Tdef   = products->T(CJdef.V,CJdef.e);
        double cdef   = products->c(CJdef.V,CJdef.e);
        double UpCdef  = CJdef.u + cdef;
        double Pcb  = products->P(shock.V,shock.e);
        double Tcb  = products->T(shock.V,shock.e);
        double ccb  = products->c(shock.V,shock.e);
        double UpC  = shock.u + ccb;
        cout        << setw(9) << setprecision(2) << P
             << " " << setw(9) << setprecision(2) << shock.us
             << " " << setw(9) << setprecision(2) << Tshock
             << " " << setw(9) << setprecision(2) << CJdef.P
             << " " << setw(9) << setprecision(2) << CJdef.us
             << " " << setw(9) << setprecision(2) << Tdef
             << " " << setw(9) << setprecision(2) << UpCdef
             << " " << setw(9) << setprecision(2) << Pcb
             << " " << setw(9) << setprecision(2) << Tcb
             << " " << setw(9) << setprecision(2) << ccb
             << " " << setw(9) << setprecision(2) << UpC
             << "\n";
        delete def;
    }
    delete hug;
    delete det;
    deleteEOS(products);
    deleteEOS(reactants);
    deleteEOS(eos);
    return 0;
}
