#include <Arg.h>
//#include "/home/woodscn/git-repos/eoslib/data_path.h"
#include "DataBase.h"
#include <sstream>

using namespace std;
int main(int, char **argv)
{
    ProgName(*argv);
    //    auto Files = "/home/woodscn/git-repos/eoslib/DATA/Units.data";
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

    std::ostringstream test1;
    std::ostringstream test2;
    std::ostringstream test3;

    test1 << units << "\n";
    Units *u2 = (Units *)db.FetchObj("Units",type,name);
    test2 << u2 << "\n";
    Units *u3 = (Units *)db.FetchNewObj("Units",type,name);
    test3 << u3 << "\n";

    if(test1.str() != test2.str() || test1.str() == test3.str()){return 1;}
    
    deleteUnits(u2);
    deleteUnits(units);
    deleteUnits(u3);

    return 0;
}

