#ifndef CCLIB_ARG_H_
#define CCLIB_ARG_H_

#include <sstream>
#include <cstring>
#include <LocalIo.h>


int inline SetVar(char **&argv, const char *s, double& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}
		
int inline SetVar(char **&argv, const char *s, int& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, unsigned int& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, short int& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, unsigned short int& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, long int& var)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, char*& name)
{
	if(!strcmp(*argv, s))
	{
		if(!(*++argv))
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		name = strdup(*argv);
		
		if(!name)
			cerr << Error << "Command line error after `" <<
				s << "' (Memory allocation failed)" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVar(char **&argv, const char *s, const char*& name)
{
	if(!strcmp(*argv, s))
	{
		if(!(*++argv))
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		name = strdup(*argv);
		
		if(!name)
			cerr << Error << "Command line error after `" <<
				s << "' (Memory allocation failed)" << Exit;
		return 1;
	}
	return 0;
}

int inline SetVarValue(char **&argv, const char *s, double& var, double val)
{
	if(!strcmp(*argv, s))
	{
		var = val;
		return 1;
	}
	else
		return 0;
}
		
int inline SetVarValue(char **&argv, const char *s, int& var, int val)
{
	if(!strcmp(*argv, s))
	{
		var = val;
		return 1;
	}
	else
		return 0;
}


int inline SetVar2(char **&argv, const char *s, double& var1, double& var2)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var1) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
							
		if( !(istringstream(*++argv) >> var2) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	
	return 0;
}

int inline SetVar2(char **&argv, const char *s, int& var1, int& var2)
{
	if(!strcmp(*argv, s))
	{
		if( !(istringstream(*++argv) >> var1) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
							
		if( !(istringstream(*++argv) >> var2) )
			cerr << Error << "Command line error after `" <<
							s << "'" << Exit;
		return 1;
	}
	
	return 0;
}


#define GetVarValue(name, var, val) \
			if( SetVarValue(argv, #name, var, val) ) continue
	
#define GetVar(name, var) \
			if(SetVar(argv, #name, var)) continue

#define GetVar2(name, var1, var2) \
			if( SetVar2(argv,#name, var1, var2) ) continue
			
			
#define ArgError	cerr << Error << "Unknown Command line argument `" << \
				*argv << "'" << Exit
				
#define EndArg		break



/* Example of Usage:
	
		
// process args using argument line : Limits num num
//	and n_dir num

	while(*++argv)
	{
		GetVar2(Limits, phi_min, phi_max);
		GetVar(n_dir, n_dir);
		ArgError;
	}
*/

#endif /* CCLIB_ARG_H_ */


