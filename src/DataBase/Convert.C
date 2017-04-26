#include <iomanip>
#include <Arg.h>
#include <LocalMath.h>

#include "DataBase.h"
#include <iostream>
using namespace std;

const char *help[] = {    // list of commands printed out by help option
    "file[s]     file    # colon separated list of data files",
    "                    #      default: $EOSLIB_DATA_PATH/Units.data",
    "lib         name    # directory for EOSlib shared libraries",
    "                    #      default: $EOSLIB_SHARED_LIBRARY_PATH",
    "type        name    # units type [hydro]",
    "",
    "from        name    # from units",
    "to          name    # to units",
    "",
    "list of units to convert",
    0
};

void Help(int status)
{
    const char **list;
    for(list=help ;*list; list++)
    {
        cerr << *list << "\n";
    }
    exit(status);
}

int main(int, char **argv)
{
    ProgName(*argv);

    const char *files = NULL;
    const char *lib   = NULL;

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

        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
            
        break;
    }
    
    // input check
    if( files == NULL )
    {
        char *DATA_dir = getenv("EOSLIB_DATA_PATH");
        if( DATA_dir==NULL )
        {
            std::cerr << Error("must specify database file\n");
        }
        string str(DATA_dir);
        str += "/Units.data";
        files = strdup(str.c_str());
    }

    if( lib )
    {
        setenv("EOSLIB_SHARED_LIBRARY_PATH",lib,1);
    }
    else if( !getenv("EOSLIB_SHARED_LIBRARY_PATH") )
    {
        cerr << Error("must specify lib or export EOSLIB_SHARED_LIBRARY_PATH")
             << Exit;  
    }
    
    if( from == NULL )
    {
        std::cerr << Error("must specify from\n");
    }
    DataBase db;
    if( db.Read(files) )
    {
        std::cerr << Error("db.Read failed\n");    
    }
    
    Units *From = db.FetchUnits(type,from);
    if( From == NULL )
        std::cerr << Error("Failed to fetch from = ") << from << Exit;

    if( to == NULL )
    {
        for( ; *argv; ++argv)
            cout << *argv << " " << From->Unit(*argv) << "\n";
        deleteUnits(From);
        return 0;
    }

    Units *To   = db.FetchNewUnits(type,to);
    if( To == NULL )
        std::cerr << Error("Failed to fetch to = ") << to << Exit;
    
    Convert convert(*From, *To);
    if( convert )
        std::cerr << Error("Bad convert status\n") << Exit;
    
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
