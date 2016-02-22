#include <LocalIo.h>
#include <Arg.h>

#include <DnKappa.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "test.data";
	const char *type = "ScaledDn";
	const char *name = "PBX9502";
	const char *units = NULL;
 
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        	
		ArgError;
	}
	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

    DnKappa *PBX = FetchDnKappa(type,name, db);
	if( PBX == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && PBX->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    PBX->Print(cout);
    deleteDnKappa(PBX);
    return 0;
}
