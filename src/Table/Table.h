#ifndef EOSLIB_TABLE_H
#define EOSLIB_TABLE_H
//
#include <iosfwd>
using std::ostream;
#include "EOSbase.h"
//
#define Table_vers "Tablev2.0.1"
#define Table_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Table_lib_vers;
extern const char *Table_lib_date;

inline int Table_lib_mismatch()
    { return strcmp(Table_vers, Table_lib_vers); }
inline int Table_lib_mismatch(const char *vers)
    { return strcmp(vers, Table_lib_vers);       }
//
//
extern "C" { // dynamic link functions for database
    char *Table_Init();
    char *Table_Info();
}

class Table : public EOSbase            // abstract base class
{
public:
    static int init;
    static void Init();
 // misc
    virtual ~Table();    
    Table *Duplicate()      {return static_cast<Table*>(EOSbase::Duplicate());} 
private:
    Table();                                  // disallowed
    Table *operator &();                      // disallowed, use Duplicate()
    void operator=(const Table&);             // disallowed
protected:
    Table(const char *ptype);
    Table(const Table&);
    int Copy(const Table&, int check = 1);
    //
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
public:
    virtual int AsciiTable(const char *file);
    virtual int BinaryTable(const char *file);
    virtual int TreeTable(const char *dir);
};
//
inline int deleteTable(Table *&tbl)
{
    int status = deleteEOSbase(tbl,"Table");
    tbl = NULL;
    return status;
}
inline ostream & operator<<(ostream &out, Table &tbl)
            { return tbl.Print(out); }
//
// Database
// 
inline Table *FetchTable(const char *type, const char *name, DataBase &db)
        { return (Table *) db.FetchObj("Table",type,name);}
inline Table *FetchNewTable(const char *type, const char *name, DataBase &db)
        { return (Table *) db.FetchNewObj("Table",type,name);}
inline Table *FetchTable(const char *type_name, DataBase &db)
        { return (Table *) db.FetchObj("Table",type_name);}
inline Table *FetchNewTable(const char *type_name, DataBase &db)
        { return (Table *) db.FetchNewObj("Table",type_name);}

#endif    // EOSLIB_TABLE_H
