#include <Arg.h>
#include <String.h>
#include <Table.h>

int main(int, char **argv)
{
    ProgName(*argv);
    Table::Init();

    const char *files    = "test.data";    
    const char *type     = "MatToSesame";
    const char *name     = "TATB";
    const char *units    = "hydro::std";
    const char *lib      = NULL;
    const char *EOSlog   = "EOSlog";      // EOS error log file
    //
    int print      = 0;             // flag to print table parameters
    char *out      = "AsciiTable";  // output file name

    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(units,units);
	    GetVar(lib,lib);
        GetVar(EOSlog,EOSlog);
        if( !strcmp(*argv,"-EOSlog") )
        {
            EOSlog = NULL;
            continue;
        }
        GetVar(out,out);
        GetVarValue( print, print, 1);
        GetVarValue( printall, print, 2);
        //
        ArgError;
    }
    if( lib )
    {
#ifdef LINUX
    setenv("SharedLibDirEOS",lib,1);
#else
    putenv(Cat("SharedLibDirEOS=",lib));
#endif
    }
// fetch table
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    if( EOSlog )
    {
        ofstream *EOS_log = new ofstream(EOSlog);
        EOSbase::ErrorDefault = new EOSbase::Error(*EOS_log);
    }
    Table *table = FetchTable(type,name,db);
    if( table == NULL )
        cerr << Error("FetchTable failed for ") << type << "::" << name << Exit;    
    if( table->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    //    
    if( print == 1 )
    {
        cout << *table;
    }
    else if( print == 2)
    {
        table->PrintAll(cout);
    }
    else
    {
        int status = table->AsciiTable(out);
        if( status )
        {
            cerr << Error("AsciiTable failed with status ") << status << Exit;
            return 1;
        }
    }
    deleteTable(table);
    return 0;
}
