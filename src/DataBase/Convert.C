#include <iomanip>
#include <Arg.h>
#include <LocalMath.h>

#include "DataBase.h"

int main(int, char **argv)
{
    ProgName(*argv);
    const char *files = NULL;
    const char *type = "hydro";
    const char *from = NULL;
    const char *to = NULL;
        
    while(*++argv)
    {
        GetVar(files,files);
        GetVar(file,files);
        GetVar(type,type);
        GetVar(from,from);
        GetVar(to,to);
            
        break;
    }
    
    int status = 0;
    if( files == NULL )
    {
        status = 1;
        cerr << Error("must specify database file\n");
    }
    if( from == NULL )
    {
        status = 1;
        cerr << Error("must specify from\n");
    }
    DataBase db;
    if( db.Read(files) )
    {
        status = 1;
        cerr << Error("db.Read failed\n");    
    }
    if( status )
        cerr << Error("initialization failed") << Exit;
    
    Units *From = db.FetchUnits(type,from);
    if( From == NULL )
        cerr << Error("Failed to fetch from = ") << from << Exit;

    if( to == NULL )
    {
        for( ; *argv; ++argv)
            cout << *argv << " " << From->Unit(*argv) << "\n";
        deleteUnits(From);
        return 0;
    }
        
    
    Units *To   = db.FetchNewUnits(type,to);
    if( To == NULL )
        cerr << Error("Failed to fetch to = ") << to << Exit;
    
    Convert convert(*From, *To);
    if( convert )
        cerr << Error("Bad convert status") << Exit;
    
    for( ; *argv; ++argv)
    {    
        double factor = convert.factor(*argv);
        cout << *argv
             << " "    << From->Unit(*argv)
             << " -> " << To->Unit(*argv)
             << " times " << factor << "\n";
    }

    deleteUnits(From);
    deleteUnits(To);

    return 0;
}
