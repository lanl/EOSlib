#include <Arg.h>
#include "DataBase.h"

int main(int, char **argv)
{
    ProgName(*argv);
    const char *files = "Units.data";
    const char *type = "hydro";
    const char *name = "std";
        
    while(*++argv)
    {
        GetVar(files,files);
        GetVar(file,files);
        GetVar(type,type);
        GetVar(name,name);
            
        ArgError;
    }
    
    DataBase db;
    if( db.Read(files) )
        cerr << Error("db.Read failed\n") << Exit;    
    
    Units *units = (Units *)db.FetchObj("Units",type,name);
    if( units == NULL )
        cerr << Error("db.FetchObj failed\n") << Exit;
    
    units->Print(cout);
    cout << "units " << units << "\n";
    Units *u2 = (Units *)db.FetchObj("Units",type,name);
    cout << "again " << u2 << "\n";
    Units *u3 = (Units *)db.FetchNewObj("Units",type,name);
    cout << "  new " << u3 << "\n";

    
    deleteUnits(u2);
    deleteUnits(units);
    deleteUnits(u3);

    return 0;
}

