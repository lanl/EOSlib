#include <Arg.h>
#include <string>

#include "DataBase.h"
using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	const char *files = NULL;
	const char *base = "";
	const char *type = NULL;
	const char *name = NULL;
		
	int print = 0;
	
	while(*++argv)
	{
		GetVar(files,files);
		GetVar(file,files);
		GetVar(base,base);
		GetVar(type,type);
		GetVar(name,name);
        GetVarValue(plain,print,-1);
		GetVarValue(parameters, print, 1);
		GetVarValue(use, print, 2);
			
		ArgError;
	}

    int status = 0;
    if( files == NULL )
    {
        status = 1;
        cerr << Error("must specify database file\n");
    }
    if( base == NULL )
    {
        status = 1;
        cerr << Error("must specify base\n");
    }
	DataBase db;
	if( db.Read(files) )
	{
        status = 1;
		cerr << Error("db.Read failed\n");	
	}
    if( status )
        cerr << Error("initialization failed\n") << Exit;

    string str;
    str = str + base;
    if(type) str = str + ":" + type;
    if(name) str = str + "::" + name;
    char *property = strdup(str.c_str());
    db.List(cout,property,print);
    
	return 0;
}
    
