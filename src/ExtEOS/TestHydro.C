#include <LocalIo.h>
#include <Arg.h>

#include <ExtEOS.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "../../DATA/EOS.data";
	const char *type = "Hayes";
	const char *name = "HMX";
	const char *units = NULL;
    const char *lib = "../lib/Linux";

    EOS::Init();
    double V = EOS::NaN;
    double e = EOS::NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
		GetVar(lib,lib);
        
		GetVar(V,V);
		GetVar(e,e);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	ExtEOS *eos = FetchExtEOS(type,name,db);
	if( eos == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    if( !std::isnan(V) && !isnan(e) )
    {
        double P = eos->P(V,e);
        cout << "P = " << P << "\n";
        double T = eos->T(V,e);
        cout << "T = " << T << "\n";
        double S = eos->S(V,e);
        cout << "S = " << S << "\n";
        double c = eos->c(V,e);
        cout << "c = " << c << "\n";
        eos->c2_tol = 1e-6;
        double c2 = eos->EOS::c2(V,e);
        cout << "EOS::c = " << sqrt(max(0.,c2)) << "\n";
    }
    else    
    {
	    eos->PrintAll(cout);

        ExtEOS *eos1 = eos->Copy();
        // Test of copy
        if( eos1 == NULL )
            cerr << Error("Copy() failed") << Exit;
        double z[1];
        deleteExtEOS(eos1);
        eos1 = eos->Copy(z);
        if( eos1 == NULL )
            cerr << Error("Copy(z) failed") << Exit;
    }

    deleteExtEOS(eos);
    return 0;
}
