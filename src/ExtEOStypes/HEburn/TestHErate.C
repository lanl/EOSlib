#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"


int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "Arrhenius";
    const char *name = "HMX";
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

    HErate *rate = FetchHErate(type,name,db);
    if( rate == NULL )
        cerr << Error("FetchHErate failed") << Exit;
    if( units && rate->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;

    const char *info = db.BaseInfo("HErate");
    if( info == NULL )
        cerr << Error("No HErate info\n");
    else
        cout << "Base info\n"
             << info << "\n";

    info = db.TypeInfo("HErate",type);
    if( info == NULL )
        cerr << Error("No type info\n");
    else
        cout << "Type info\n"
             << info << "\n";
    rate->PrintAll(cout);
    deleteHErate(rate);
    return 0;
}
