#ifndef CCLIB_ERROR_HANDLER_H
#define CCLIB_ERROR_HANDLER_H


#include <iosfwd>
#include <cstdlib>
#include <cstdarg>
//using namespace std;


// ToDo: Use exceptions
//          Library: LocalErrorHandler::Abort() aborts with throw *this;
//      Application: try{ . . . } catch(LocalErrorHandler db) { . . . }
class LocalErrorHandler		// base class for DataBase error handler
{
public:
    std::ostream *out;
    LocalErrorHandler()             : out(NULL) { }
    LocalErrorHandler(std::ostream &Out) : out(&Out) { }
    virtual ~LocalErrorHandler();
    virtual void Log(const char *func, const char *format, ...);
    virtual void Error(const char *func, const char *file, int line,
		       const char *format, va_list args);
    virtual void Throw(const char *func, const char *file, int line,
		       const char *format, ...);
    virtual void Abort(const char *func, const char *file, int line,
		       const char *format, ...);
};

extern LocalErrorHandler *LocalError; // standard default error handler


#endif // CCLIB_ERROR_HANDLER_H

