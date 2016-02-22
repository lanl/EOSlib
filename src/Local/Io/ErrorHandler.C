#include <cstdio>
#include <iostream>
#include <stdarg.h>

#include "ErrorHandler.h"


LocalErrorHandler *LocalError = new LocalErrorHandler(cerr);

LocalErrorHandler::~LocalErrorHandler()
{
	// NULL
}


#define MAX 1024

void LocalErrorHandler::Log(const char *func, const char *format, ...)
{
	if( out )
	{
	   *out << "LocalErrorHandler called from " << func << ", ";    
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << "\n";
	   out->flush();
	}
}

void LocalErrorHandler::Abort(const char *func, const char *file, int line,
		              const char *format, ...)
{
	if( out )
	{
	   *out << "Abort in " << func << ", file `" << file
	        << "' line " << line << endl;	   
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << endl;
	}
	
	exit(1);
}

void LocalErrorHandler::Throw(const char *func, const char *file, int line,
		              const char *format, ...)
{
	if( out )
	{
	   *out << "LocalErrorHandler exception in "
		<< func << ", file `" << file
	        << "' line " << line << endl;	   
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << endl;
	}
	
	throw *this;
}
