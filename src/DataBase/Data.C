#include <Arg.h>
#include <string>

#include "DataBase.h"
using namespace std;

const char *help[] = {    // list of commands printed out by help option
    "file[s]     file    # colon separated list of data files",
    "base        name    # base class ['']",
    "type        name    # derived class type ['*']",
    "name        name    # property name ['*']",
    
    "plain               # print only base:type::name",
    "parameters          # print parameters",
    "use                 # substitute use directives",
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
	const char *base = "";
	const char *type = "*";
	const char *name = "*";
		
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
			
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
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
    
