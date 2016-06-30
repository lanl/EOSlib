#include <Arg.h>
#include <LocalMath.h>
#include "EOS.h"

#ifdef NAN
  #define NaN NAN
#else
  #define NaN EOS::NaN
#endif

int MAX_ITR     = 20;
int MAX_SUB_ITR = 20;

int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    Format G_form;
    Format CV_form;
    Format beta_form;

    const char *file     = "HE.data";
    const char *type     = NULL;
    const char *name     = NULL;
	const char *material = NULL;
	const char *units    = "hydro::std";

    double rho = 0.0;
    double V = NaN;
    double P = NaN;
    double e = NaN;
    
    double abs_tol = -1.;
    double rel_tol = -1.;


    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(type,type);
        GetVar(name,name);
	    GetVar(material,material);
        GetVar(units,units);
        
        GetVar(rho,rho);
        GetVar(V,V);
        GetVar(P,P);
        GetVar(e,e);
        

        GetVar(abs_tol,abs_tol);
        GetVar(rel_tol,rel_tol);


	    if( !strcmp(*argv, "e_form") )
	    {
	        char *ptr;
	        e_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        e_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }
        
        ArgError;
    }
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);

    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
	if( material )
	{
	    if( type || name )
		    cerr << Error("Can not specify both material and name or type")
	             << Exit;
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material
		         << Exit;
	}
    if( type==NULL || name==NULL )
        cerr << Error("type or name note specified") << Exit;

    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read data file failed" ) << Exit;

    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    double V0 = eos->V_ref;
    double e0 = eos->e_ref;
    double P0 = eos->P(V0,e0);

    if( rho > 0.0 )
        V = 1/rho;
    else if( std::isnan(V) )
        V = V0;
    if( std::isnan(P) )
        P = P0;

    // initial guess for e such that (V,e) in domain
    if( std::isnan(e) )
    {
        e = e0;
        if( V<V0)
            e += P0*(V0-V);
        double T = eos->T(V,e);
        if( T<=0.0 || eos->NotInDomain(V,e) )
        {
            HydroState state0(V0,e);
            Isentrope *S0 = eos->isentrope(state0);
            if( S0 == NULL )
                cerr << Error("Isentrope(state0) failed") << Exit;
            WaveState stateV;
            if( S0->V(V, RIGHT, stateV) )
                cerr << Error("S0->V failed") << Exit;
            e = stateV.e;
            delete S0;
            T = eos->T(V,e);
            if( T <= 0.0 )
                cerr << Error("T(V,S0) < 0, bad EOS") << Exit;
        }
    }
    // iteration
    if( abs_tol <= 0.0 )
        abs_tol = eos->OneDFunc_abs_tol;
    if( rel_tol <= 0.0 )
        rel_tol = eos->OneDFunc_rel_tol;
    double tol = max(abs_tol, rel_tol*P);
    int count=MAX_ITR;
    int k;
    double P1;
    while( count-- )
    {
        P1 = eos->P(V,e);
        double dP = P1 - P;
        if( abs(dP) < tol )
            break;
        double Gamma = eos->Gamma(V,e);
        double T =  eos->T(V,e);
        double Cv = eos->CV(V,e);
        double de1, de2;
        k=0;
        if( Gamma <= 0.0 )
            de1 = HUGE_VAL;
        else
        {
            de1 = abs(dP)*V/Gamma;
            k = MAX_SUB_ITR;
        }
        if( T <= 0.0 || Cv <= 0.0 )
            de2 = HUGE_VAL;
        else
        {
            de2 = T/Cv;
            k = MAX_SUB_ITR;
        }
        double de;
        if( P1 > P )
        {
            de = min( de1, 0.1*de2 );
            while( k-- )
            {
               if( eos->InDomain(V,e-de) )
                   break;
               de *= 0.5;
            }
        }
        else
        {
            de = -min( de1, 0.5*de2 );
            while( k-- )
            {
               if( eos->InDomain(V,e-de) )
                   break;
               de *= 0.5;
            }
        }
        if( k < 0 )
        {
            cout << "*** Not in Domain"
                    ", iteration " << MAX_ITR-count
                 << ", sub iteration " << MAX_SUB_ITR-k
                 << " ***\n";
            cout << "    Gamma=" << Gamma
                 << ", Cv="    << Cv
                 << ", T="     << T
                 << "\n";
            count = -1;
            break;
        }
        e -= de;
    }
    if( count < 0 )
        cout << "*** Iteration failed to converge ***\n";

    double T  = eos->T(V,e);
    double c  = eos->c(V,e);
    double cT = sqrt( max(0.0,V*eos->KT(V,e)) );
    double Gamma = eos->Gamma(V,e);
    double Cv = eos->CV(V,e);
    double beta = eos->beta(V,e);

    
    cout << "material " << type<<"::"<<name << "\n";
    cout << Center(V_form, "V")     << " "
         << Center(e_form, "e")     << " "
         << Center(P_form, "P")     << " "
         << Center(T_form, "T")     << " "
         << Center(c_form, "c")     << " "
         << Center(c_form, "cT")    << " "
         << Center(G_form, "Gamma") << " "
         << Center(CV_form, "Cv")   << " "
         << Center(beta_form, "beta") << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        cout << Center(V_form,u->Unit("V"))   << " "
             << Center(e_form,u->Unit("e"))   << " "
             << Center(P_form,u->Unit("P"))   << " "
             << Center(T_form,u->Unit("T"))   << " "
             << Center(u_form,u->Unit("u"))   << " "
             << Center(u_form,u->Unit("u"))   << " "
             << Center(G_form,"-")            << " "
             << Center(CV_form,u->Unit("Cv")) << " "
             << Center(beta_form,u->Unit("beta")) << "\n";
    }
    cout.fill('-');
    cout << setw(V_form.width)  << "" << "-"
         << setw(e_form.width)  << "" << "-"
         << setw(P_form.width)  << "" << "-"
         << setw(T_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(G_form.width)  << "" << "-"
         << setw(CV_form.width) << "" << "-"
         << setw(beta_form.width) << "" << "\n";
    cout.fill(' ');
    cout << V_form  << V  << " "
         << e_form  << e  << " "
         << P_form  << P1 << " "
         << T_form  << T  << " "
         << c_form  << c  << " "
         << c_form  << cT << " "
         << G_form  << Gamma << " "
         << CV_form << Cv << " "
         << beta_form << beta << "\n";

    deleteEOS(eos);
    return 0;
}
