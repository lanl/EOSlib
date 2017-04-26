#include <Arg.h>
#include <LocalMath.h>
#include "ExtEOS.h"
//
int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file     = "Test.data";

    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
	
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    double lambda = 0.55;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(lambda,lambda);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;

    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ExtEOS *xeos = dynamic_cast<ExtEOS*>(eos);
    if( xeos == NULL )
        cerr << Error("xeos is NULL") << Exit;
    xeos->Print(cout);
    
    double z[1] = {lambda};    
    ExtEOS *ceos = xeos->Copy(z);
    ceos->Print(cout << "\ncopy\n\n");
    
    deleteEOS(eos);
    return 0;
}
