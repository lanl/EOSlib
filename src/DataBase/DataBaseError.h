#ifndef EOSLIB_DATA_BASE_ERROR_H
#define EOSLIB_DATA_BASE_ERROR_H

#include <iosfwd>
using std::ostream;

class DataBase;

// Exceptions
//          Library: db->DB_error::handler() aborts with throw *this;
//      Application: try{ . . . } catch(DataBase_Error db) { . . . }
//
// TODo: Add
/*
 * char *last_message;
 * enum ACTION {ABORT, LOG, THROW, WARN};
 * int action;
 * int ret_value;
 *      action = -1, abort
 *             =  0, log
 *             =  2, throw
 *             =  3, WARN = log
 *             =  4, WARN = THROW
 *             =  5, WARN = ABORT
 *             =  6, WARN = ignore
 *  virtual int Error(int severity, const char *func, const char *file,
 *                     int line, DataBase *db, const char *format, ...);
#define Warn(func) WARN,"DataBase::" #func,__FILE_,__LINE__, this
*/
// usage in library routine:
// return Error(Warn(function),"message", . . . );                   
class DataBase_Error        // base class for DataBase error handler
{
public:
    DataBase *DB;
    ostream *out;
    DataBase_Error()             : DB(NULL), out(NULL) { }
    DataBase_Error(ostream &Out) : DB(NULL), out(&Out) { }
    virtual ~DataBase_Error();
    virtual void Log(const char *func, const char *file, int line,
                 DataBase *db, const char *format, ...);
    virtual void Abort(const char *func, const char *file, int line,
                 DataBase *db, const char *format, ...);
    virtual void Throw(const char *func, const char *file, int line,
                 DataBase *db, const char *format, ...);
};

#endif // EOSLIB_DATA_BASE_ERROR_H
