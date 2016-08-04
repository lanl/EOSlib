#include <Arg.h>
#include <EOS.h>

using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsEOS.data";
    const char * file = file_.c_str();
    //const char *file = NULL;
    const char *type = NULL;
    const char *name = NULL;
    const char *units = "hydro::std";
    //const char *material = NULL;
    const char *material = "BirchMurnaghan::HMX";
    int printu = 0;
    int all = 0;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(units,units);
        GetVar(material,material);
        GetVarValue(printu,printu,1);
        GetVarValue(all,all,1);
            
        ArgError;
    }
    
    if( file == NULL )
        file = "EOS.data";
        
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
    if( db.Read(file) )
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
