#include <Arg.h>
#include "data_path.h"
#include "DataBase.h"
using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    auto Files = data_path + "/DATA/Units.data";
    const char *files = Files.c_str();
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

