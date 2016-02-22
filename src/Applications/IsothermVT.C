#include <Arg.h>
#include <EOS.h>
#include <EOS_VT.h>


#define NaN EOS::NaN
int main(int, char **argv)
{
	ProgName(*argv);
    EOS_Init();
    Format G_form;
    Format CV_form;
    Format beta_form;
	
	int nsteps = 10;
	
	double V0 = NaN;
	double T0 = NaN;
	
	const char *files    = "EOS.data";    
	const char *type     = NULL;
	const char *name     = NULL;
	const char *material = NULL;
	const char *units    = "hydro::std";
	
	double var1 = NaN;
	double var2 = NaN;
	
	
// process command line arguments
	while(*++argv)
	{
	    GetVar(file,files);
	    GetVar(files,files);
	    GetVar(type,type);
	    GetVar(name,name);
	    GetVar(material,material);
	    GetVar(units,units);
	    
	    GetVar(V0,V0);
	    GetVar(T0,T0);

	    GetVar2(range,var1,var2);
	    GetVar(var1,var1);
	    GetVar(var2,var2);
	    GetVar(nsteps,nsteps);
	    
	    if( !strcmp(*argv, "V_form") )
	    {
	        char *ptr;
	        V_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        V_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }
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
	    if( !strcmp(*argv, "u_form") )
	    {
	        char *ptr;
	        u_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        u_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }

	    ArgError;
	}
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	
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
		 cerr << Error("name or type not set") << Exit;

// set eos and initial state
	DataBase db;
	if( db.Read(files) )
		cerr << Error("Read failed" ) << Exit;
	
	EOS *eos = FetchEOS(type,name,db);
	if( !eos )
	    cerr << Error("material not found, ") << type << "::" << name << Exit;    
	if( eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    EOS_VT *VT = eos->eosVT();
    if( VT==NULL )
		cerr << Error("eosVT failed") << Exit;

    if( isnan(V0) )
        V0 = VT->V_ref;
    if( isnan(T0) )
        T0 = VT->T_ref;
    if( isnan(var1) )
        var1 = V0;
    if( isnan(var2) )
        var2 = V0;

    cout << "EOS_VT isotherm for " << type<<"::"<<name << " at T = " << T0 << "\n";
    cout << Center(V_form, "V")     << " "
         << Center(e_form, "e")     << " "
         << Center(P_form, "P")     << " "
         << Center(c_form, "c")     << " "
         << Center(c_form, "cT")    << " "
         << Center(G_form, "Gamma") << " "
         << Center(CV_form, "Cv")   << " "
         << Center(beta_form, "beta") << " "
         << Center(T_form, "Teos") << " "
         << Center(P_form, "Peos") << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        cout << Center(V_form,u->Unit("V"))   << " "
             << Center(e_form,u->Unit("e"))   << " "
             << Center(P_form,u->Unit("P"))   << " "
             << Center(u_form,u->Unit("u"))   << " "
             << Center(u_form,u->Unit("u"))   << " "
             << Center(G_form,"-")            << " "
             << Center(CV_form,u->Unit("Cv")) << " "
             << Center(beta_form,u->Unit("beta")) << " "
             << Center(T_form,u->Unit("T"))   << " "
             << Center(P_form,u->Unit("P"))   << "\n";
    }
    cout.fill('-');
    cout << setw(V_form.width)  << "" << "-"
         << setw(e_form.width)  << "" << "-"
         << setw(P_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(G_form.width)  << "" << "-"
         << setw(CV_form.width) << "" << "-"
         << setw(beta_form.width) << "" << "-"
         << setw(T_form.width)  << "" << "-"
         << setw(P_form.width)  << "" << "\n";
    cout.fill(' ');

    int i=0;
    if( var1 == var2 )
    {
        nsteps = 1;
        i = 1;
    }
    double Nsteps = nsteps;
    for( ; i<=nsteps; i++ )
    {
        double I = i;
        double V = ((Nsteps-I)*var1+I*var2)/Nsteps;
        double e = VT->e(V,T0);
        double P = VT->P(V,T0);
        double c = VT->c(V,T0);
        double cT = VT->cT(V,T0);
        double Gamma = VT->Gamma(V,T0);
        double Cv = VT->CV(V,T0);
        double beta = VT->beta(V,T0);
        // check
        double Peos = eos->P(V,e);
        double Teos = eos->T(V,e);
        cout << V_form  << V  << " "
             << e_form  << e  << " "
             << P_form  << P  << " "
             << c_form  << c  << " "
             << c_form  << cT << " "
             << G_form  << Gamma << " "
             << CV_form << Cv << " "
             << beta_form << beta << " "
             << T_form  << Teos << " "
             << P_form  << Peos << "\n";
    }

// test reference counting
	deleteEOS_VT(VT);
	deleteEOS(eos);
	    
	return 0;
}
