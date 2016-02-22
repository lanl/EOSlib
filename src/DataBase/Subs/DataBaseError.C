#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdarg>
using namespace std;


#include "DataBaseError.h"


DataBase_Error::~DataBase_Error()
{
	// NULL
}


#define MAX 1024

void DataBase_Error::Log(const char *func, const char *file, int line,
		         DataBase *db, const char *format, ...)
{
	DB = db;
	if( out )
	{
	   *out << "DataBase_error_handler::Log, " << func << ", file `"
		<< file << "' line " << line << endl;	   
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << endl;
	}
}

void DataBase_Error::Abort(const char *func, const char *file, int line,
		           DataBase *db, const char *format, ...)
{
	DB = db;
	if( out )
	{
	   *out << "DataBase_error_handler::Abort, " << func << ", file `"
		<< file << "' line " << line << endl;	   
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << endl;
	}
	exit(1);
}

void DataBase_Error::Throw(const char *func, const char *file, int line,
		           DataBase *db, const char *format, ...)
{
	DB = db;
	if( out )
	{
	   *out << "DataBase_error_handler::Throw, " << func << ", file `"
		<< file << "' line " << line << endl;	   
	
	   va_list args;
	   va_start(args,format);
	   char line[MAX];
	   (void) vsnprintf(line, MAX, format, args);
	   va_end(args);
	   *out << line << endl;
	}
	throw *this;
}

