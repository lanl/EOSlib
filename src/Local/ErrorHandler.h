#ifndef CCLIB_ERROR_HANDLER_H
#define CCLIB_ERROR_HANDLER_H


#include <iosfwd>
#include <cstdlib>
using namespace std;


// ToDo: Use exceptions
//          Library: LocalErrorHandler::Abort() aborts with throw *this;
//      Application: try{ . . . } catch(LocalErrorHandler db) { . . . }
class LocalErrorHandler		// base class for DataBase error handler
{
public:
    ostream *out;
    LocalErrorHandler()             : out(NULL) { }
    LocalErrorHandler(ostream &Out) : out(&Out) { }
    virtual ~LocalErrorHandler();
    virtual void Log(const char *func, const char *format, ...);
    virtual void Abort(const char *func, const char *file, int line,
		       const char *format, ...);
    virtual void Throw(const char *func, const char *file, int line,
		       const char *format, ...);
};

extern LocalErrorHandler *LocalError; // standard default error handler


#endif // CCLIB_ERROR_HANDLER_H

