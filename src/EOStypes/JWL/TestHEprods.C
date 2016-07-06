#include <Arg.h>

#include <EOS.h>
#include <IO.h>
#include "HEprods.h"

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "HEprods";
	const char *name = "PBX9502";
	const char *units = NULL;

    new HEprods(); // force EOS::NaN to be allocated 
    double V = EOS::NaN;
    double e = EOS::NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(e,e);
		
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


    eos->Print(cout);

    deleteEOS(eos);
    return 0;
}

