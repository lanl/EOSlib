#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"

int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/HE.data";
    const char * file = file_.c_str();
 

    //const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.BM";
    const char *units = NULL;
    const char *lib   = "../../lib/Linux";
    int nsteps  = 10;
    int state   = 0;
    double lambda_min = 0.0;
    double lambda_max = 1.0;
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
        GetVar(lambda_min,lambda_min);
        GetVar(lambda_max,lambda_max);
        GetVarValue(CJ,state,1);
        GetVarValue(init,state,0);
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
    WaveState CJ;
    HydroState state0(Vref,eref);
    Detonation *det = HE->detonation(state0,P0);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    if( state==0 )
        cout << "initial state: V=" << Vref << " e=" << eref << "\n";
    else if( state==1 )
    {
        cout << "CJ state: V=" << CJ.V << " e=" << CJ.e << "\n";
        Vref = CJ.V;
        eref = CJ.e;    
    }
    else
        cerr << Error("state must be 0 or 1") << Exit;
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# lambda"
         << " " << setw(13) << "   P"
         << " " << setw(13) << "   T"
         << " " << setw(13) << "   V1"
         << " " << setw(13) << "   e1"
         << " " << setw(13) << "   V2"
         << " " << setw(13) << "   e2"
         << " " << setw(13) << "   dP"
         << " " << setw(13) << "   dT"
         << " " << setw(13) << "   KT1"
         << "\n";    
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    double dlambda = lambda_max-lambda_min;
    for(i=0; i<=nsteps; i++)
    {
        double lambda = lambda_min + (double(i)/double(nsteps))*dlambda;
        PT->set_lambda2(lambda);
        double P = PT->P(Vref,eref);
        if( std::isnan(P) )
        {
            cout << "PT->P failed at lambda = " << lambda << "\n";
            break;
        }
        double T  = PT->T(Vref,eref);
        double dP, dT;
        if( PT->ddlambda(Vref,eref,dP,dT) )
            cerr << Error("ddlambda failed") << Exit;
        dP = -dP; dT = -dT;
        double V1 = PT->V1;
        double e1 = PT->e1;
        double V2 = PT->V2;  
        double e2 = PT->e2;
        double KT = PT->KT(Vref,eref);
        //double ct = (std::isnan(KT) || KT<0.) ? 0. : sqrt(Vref*KT);
        cout        << setw(13) << setprecision(6) << lambda
             << " " << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << V1
             << " " << setw(13) << setprecision(6) << e1
             << " " << setw(13) << setprecision(6) << V2
             << " " << setw(13) << setprecision(6) << e2
             << " " << setw(13) << setprecision(6) << dP
             << " " << setw(13) << setprecision(6) << dT
             << " " << setw(13) << setprecision(6) << KT
             << "\n";
    }
    deleteEOS(eos);
    return 0;
}
