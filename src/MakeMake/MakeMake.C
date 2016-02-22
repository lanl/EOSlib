#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
using namespace std;

#include "Depend.h"


char* base_cwd();


/*************************************/
/*************************************/

int main(int, char** argv)
{
	int exec = 0;
	char *DependFile = strdup("Depend");
	char *t_file = 0;
	char *MakeDir = 0;

	
	Dependencies Deps;
	
	Deps.macro << "PWD" ;
	Deps.macro_eval = base_cwd();

	
	while(*++argv)
	{
		if(!strcmp("EXEC", *argv))
		{
			exec = 1;
		}
		else if( !strcmp("DependFile", *argv) )
		{
			if( *++argv )
				DependFile = strdup(*argv);
			else
			{
				cerr << "Command line error after "
				     << "`DependFile'\n";
				exit(-1);
			}
		}
		else if( !strcmp("TemplateFile", *argv) )
		{
			if( *++argv )
				t_file = strdup(*argv);
			else
			{
				cerr << "Command line error after "
				     << "`TemplateFile'\n";
				exit(-1);
			}
		}
		else if( !strcmp("MakeDir", *argv) )
		{
			if( *++argv )
				MakeDir = strdup(*argv);
			else
			{
				cerr << "Command line error after "
				     << "`MakeDir'\n";
				exit(-1);
			}
		}
		else
		{
			if( Deps.Add(*argv) )
			{
				cerr << "Unknown command line argument `"
				     << *argv << "'\n";
				exit(-1);
			}
		}
	}
	
	
	
	if( exec )
		Deps.GetDep(DependFile);
	
	if(!t_file)
	{
		if(!MakeDir)
		{
			MakeDir = getenv("MAKEDIR");
		
			if(!MakeDir)
			{
			    cerr << "Need directory path for template files\n"<<
				    "\t(setenv MAKEDIR xxxxx)\n" <<
				    "\t\t or\n" << 
			            "\tCommand line argument MakeDir xxxxx\n";
				exit(-1);
			}
		}
		
        string temp(MakeDir);
        temp = temp + "/" + (exec ? "make_exec" : "make_lib");		
		t_file = strdup(temp.c_str());
	}

	Deps.ProcessTemplate(t_file, cout);
	
	return 0;
}


char* base_cwd()
{
	char *cwd = new char[128];
	
	if( getcwd(cwd, 128) == NULL )
	{
		delete cwd;
		return NULL;
	}
	
	char *base = cwd + strlen(cwd);
	
	for( ; base != cwd; base--)
	{
		if( *base == '/' )
		{
			base = strdup(base+1);
			delete cwd;
			break;
		}
	}
	
	return base;
}
