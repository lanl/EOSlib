#include <Arg.h>

#include "PhiEq.h"

using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "Phi0";
    const char *name = "estane";
    const char *units = NULL;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        ArgError;
    }

    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;

    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;

    PhiEq *phieq = FetchPhiEq(type,name,db);
    if( phieq == NULL )
        cerr << Error("FetchPhiEq failed") << Exit;
    if( units && phieq->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;

    const char *info = db.BaseInfo("PhiEq");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;
    cout << "Base info\n"
         << info << "\n";

    info = db.TypeInfo("PhiEq",type);
    if( info == NULL )
		cerr << Error("No type info" ) << Exit;
    cout << "Type info\n"
         << info << "\n";
    
    phieq->Print(cout);
    deletePhiEq(phieq);

    return 0;
}
