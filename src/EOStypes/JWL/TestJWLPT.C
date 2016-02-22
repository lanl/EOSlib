#include <Arg.h>

#include <EOS.h>
#include <IO.h>
#include "JWL.h"

int main(int, char **argv)
{
	ProgName(*argv);
	char *file = "Test.data";
	char *type = "JWL";
	char *name = "HMX";
	char *units = NULL;

    new JWL(); // force EOS::NaN to be allocated 
    double P = 0.0001;
    double T = 300;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(P,P);
		GetVar(T,T);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	EOS *eos = FetchEOS(type,name,db);
	if( eos == NULL )
	{
		cerr << "eos == NULL\n";
		cerr << Error("Fetch failed") << Exit;
	}
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    HydroState PT;
    if( eos->PT(P,T,PT) )
        cerr << Error("eos->PT failed") << Exit;

    cout << "V=" << PT.V
         << ", e=" << PT.e
         << ", P=" << eos->P(PT)
         << ", T=" << eos->T(PT)
         << "\n";

    deleteEOS(eos);
    return 0;
}

