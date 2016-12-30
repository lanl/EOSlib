#include <Arg.h>
#include <EOS.h>

using namespace std;


const char *help[] = {    // list of commands printed out by help option
    "name        name    # material name",
    "type        name    # EOS type",
    "material    name    # type::name",
    "",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "units       name    # default units from data base",
    "",
    "printu              # print units",
    "all                 # print all EOS components",
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

    const char *files    = NULL;
    const char *lib      = NULL;

    const char *type     = NULL;
    const char *name     = NULL;
    const char *units    = "hydro::std";
    const char *material = NULL;
    int printu = 0;
    int all = 0;

    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(lib,lib);

        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);

        GetVar(units,units);
        GetVarValue(printu,printu,1);
        GetVarValue(all,all,1);
            
        // help
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
        ArgError;
    }
    if( files==NULL )
        cerr << Error("must specify data file") << Exit;    
    if( lib )
    {
        setenv("EOSLIB_SHARED_LIBRARY_PATH",lib,1);
    }
    else if( !getenv("EOSLIB_SHARED_LIBRARY_PATH") )
    {
        cerr << Error("must specify lib or export EOSLIB_SHARED_LIBRARY_PATH")
             << Exit;  
    }
    
    if( material )
    {
        if( type || name )
            cerr << Error("Can not specify both material and name or type")
                 << Exit;
        if( TypeName(material,type,name) )
            cerr << Error("syntax error, material = ") << material << Exit;
    }

    if( type == NULL || name == NULL )
        cerr << Error("must specify material or type and name") << Exit;

    
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;

    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("Fetch failed for ") << type << "::" << name << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    if( all )
        eos->PrintAll(cout);
    else
        eos->Print(cout);
    if( printu )
        eos->UseUnits()->Print(cout);
    
    deleteEOS(eos);
    return 0;
}
