#include <Arg.h>
#include "DataBase.h"

using namespace std;
int main(int, char **argv)
{
    ProgName(*argv);
    const char *files = "DATA/DataBaseUnits.data";    
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

    Units *u2 = (Units *)db.FetchObj("Units",type,name);
    Units *u3 = (Units *)db.FetchNewObj("Units",type,name);

    if( u2 != units ) {return 1;}
    if( u3 == u2 )    {return 1;}

    deleteUnits(u3);
    deleteUnits(u2);
    deleteUnits(units);

    return 0;
}
