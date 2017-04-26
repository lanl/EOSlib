#include <cstdio>
#include <iostream>
#include <stdarg.h>

#include "ErrorHandler.h"

using namespace std;

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

void LocalErrorHandler::Error(const char *func, const char *file, int line,
			      const char *format, va_list args)
{
  if( out )
    {
      *out << "Error in " << func << ", file `" << file
  	   << "'line " << line << endl;
      char line[MAX];
      (void) vsnprintf(line, MAX, format, args);
      *out << line << endl;
    }
  throw *this;
}

void LocalErrorHandler::Abort(const char *func, const char *file, int line,
		              const char *format, ...)
{
  va_list args;
  va_start(args, format);
  Error(func, file, line, format, args);
  va_end(args);
}

// {
// 	if( out )
// 	{
// 	   *out << "Abort in " << func << ", file `" << file
// 	        << "' line " << line << endl;	   
	
// 	   va_list args;
// 	   va_start(args,format);
// 	   char line[MAX];
// 	   (void) vsnprintf(line, MAX, format, args);
// 	   va_end(args);
// 	   *out << line << endl;
// 	}
	
// 	exit(1);
// }

 void LocalErrorHandler::Throw(const char *func, const char *file, int line,
		              const char *format, ...)
{
  va_list args;
  va_start(args, format);
  Error(func, file, line, format, args);
  va_end(args);
}

// {
// 	if( out )
// 	{
// 	   *out << "LocalErrorHandler exception in "
// 		<< func << ", file `" << file
// 	        << "' line " << line << endl;	   
	
// 	   va_list args;
// 	   va_start(args,format);
// 	   char line[MAX];
// 	   (void) vsnprintf(line, MAX, format, args);
// 	   va_end(args);
// 	   *out << line << endl;
// 	}
	
// 	throw *this;
// }
