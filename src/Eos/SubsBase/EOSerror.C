#include <iostream>
using namespace std;
#include <cstdlib>
#include <cstdio>
#include <stdarg.h>

#include "EOSbase.h"


EOSbase::Error::Error() : eos(NULL), out(NULL)
{
    // Null
}

EOSbase::Error::Error(ostream &Out) : eos(NULL), out(&Out)
{
    // Null
}

EOSbase::Error::~Error()
{
    if( out )
        out->flush();
}


#define MAX 1024

double EOSbase::Error::Log(const char *func, const char *file, int line,
                           EOSbase *Eos, const char *format, ...)
{
    eos = Eos;
    if( out )
    {
       *out << "EOSbase::Error::Log, " << func << ", file `" << file
            << "' line " << line << endl;       
    
       va_list args;
       va_start(args,format);
       char line[MAX];
       (void) vsnprintf(line, MAX, format, args);
       va_end(args);
       *out << line << endl;
    
       if( eos )
           *out << (eos->base ? eos->base : "NULL") << " = "
                << (eos->type ? eos->type : "NULL") << "::"
                << (eos->name ? eos->name : "NULL")
                << ", status=" << eos->Status() << "\n";
       out->flush();
    }
    return EOSbase::NaN;
}

void EOSbase::Error::Abort(const char *func, const char *file, int line,
                           EOSbase *Eos, const char *format, ...)
{
    eos = Eos;
    if( out )
    {
       *out << "EOSbase::Error::Abort, " << func << ", file `" << file
            << "' line " << line << endl;       
    
       va_list args;
       va_start(args,format);
       char line[MAX];
       (void) vsnprintf(line, MAX, format, args);
       va_end(args);
       *out << line << endl;
    
       if( eos )
           *out << (eos->base ? eos->base : "NULL") << " = "
                << (eos->type ? eos->type : "NULL") << "::"
                << (eos->name ? eos->name : "NULL")
                << ", status=" << eos->Status() << "\n";
       out->flush();
    }
    exit(1);
}

void EOSbase::Error::Throw(const char *func, const char *file, int line,
                           EOSbase *Eos, const char *format, ...)
{
    eos = Eos;
    if( out )
    {
       *out << "EOSbase::Error::Throw, " << func << ", file `" << file
            << "' line " << line << endl;       
    
       va_list args;
       va_start(args,format);
       char line[MAX];
       (void) vsnprintf(line, MAX, format, args);
       va_end(args);
       *out << line << endl;
    
       if( eos )
           *out << (eos->base ? eos->base : "NULL") << " = "
                << (eos->type ? eos->type : "NULL") << "::"
                << (eos->name ? eos->name : "NULL")
                << ", status=" << eos->Status() << "\n";
       out->flush();
    }
    throw *this;
}
