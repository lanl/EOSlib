#ifndef EOSLIB_DATA_BASE_H
#define EOSLIB_DATA_BASE_H

#include <NameArray.h>

#include "DataFile.h"
#include "DataBaseParameters.h"
#include "DataBaseError.h"
#include "Handle.h"
#include "Units.h"

#define USE ":use"


typedef std::ios_base::fmtflags FmtFlags;

namespace EOSlib
{
    enum STATUS {
        good =  0,
        fail =  1,   // recovery possible
        bad  = -1,   // fatal, such as uninitialized
        abort= -2    // program error, e.g. constructor/destructor
    };    
    const char *Status(STATUS status);
}


extern DataBase_Error *DataBase_cerr;
#define DB_error DataBase_cerr

// Constitutive Property = base:type::name

class Property;
class Type;
class Base;
class DataBase;

extern "C" {
    typedef char *(*INIT)();
    typedef char *(*INFO)();
    typedef int  (*INITIALIZE)(void *, Parameters &, DataBase &);
    typedef void *(*DUPLICATE)(void *);
    typedef void (*DESTRUCTOR)(void *);
    typedef void *(*CONSTRUCTOR)();
}

int TypeName(const char *str, const char *&type, const char *&name);

//

class Property
{
private:
    Property(const Property&);                      // disallowed
    void operator=(const Property&);                // disallowed
public:
    Type *type;
    Parameters *params;
    void *obj;
    
    Property();
    ~Property();
};

class Type
{
private:
    Type(const Type&);                              // disallowed
    void operator=(const Type&);                    // disallowed
public:
    char *type;
    char *info;
    Base *base;
    CONSTRUCTOR Constructor;
    NAMEARRAY<Property> props;
    
    Type();
    Type(const char *tname);
    ~Type();
};

class Base
{
private:
    Base(const Base&);                              // disallowed
    void operator=(const Base&);                    // disallowed
public:
    char *base;
    char *info;
    INITIALIZE Initialize;
    DUPLICATE  Duplicate;
    DESTRUCTOR Destructor;
    NAMEARRAY<Type> types;
    
    Base();
    Base(const char *bname);
    ~Base();
};

class File
{
public:
    const char *file;
    File();
    File(const char *fname);
    ~File();
};

class DBvars
{
public:
    char *file;
    char *dir;
    DataFile *data;
    DBvars *prev;
    char *base;
    char *units;
    DBvars(const char *f, DBvars *p);
    ~DBvars();
};

//
class DataBase
{
public:
    enum {ERR=-1, END=0, ADD=1, NEW=2, REPLACE=3, EXIST=4};
    friend class DataBase_Error;
    friend class BaseIterator;
    friend class Units;
    static int use;                                 // count for use
    static Handle *main;
    static int init;
    static void Init();
    static DataBase_Error *error_default;  // default error handler
private:
    DataBase(const DataBase&);                      // disallowed
    void operator=(const DataBase&);                // disallowed
    const char *EnvironmentVar(const char *file);
protected:
    DataBase_Error *error;
    NAMEARRAY<Handle> handles;
    NAMEARRAY<File> Files;
    DBvars *vars;
    
    NAMEARRAY<Base> bases;
    Base *globals;   
    Base *units;
    Type *unit_values;
    Type *unit_names;
    Type *shared_libs;
    Type *database;
    
    int read(const char *file);
    int OpenFile(const char *file);
    int CloseFile();
    int DotCommands();
    int BlockId(Parameters &p);
    int ReadParameters(Parameters &p);
    int StoreParameters(int &eq, Parameters *p);
    int Use(NAMEARRAY<Property> &prop, Parameters &params,
            int count = DataBase::use);

    Base       &GetBase(const char *base);
    Type       &GetType(Base &base, const char *type);
    Type       &GetType(const char *base, const char *type)
                {return GetType(GetBase(base), type);}
    Property   &GetProp(Type &type, const char *name);
    Property   &GetProp(Parameters &p)
                {return GetProp(GetType(p.base,p.type), p.name);}
    
    Base       *FetchBase(const char *base);
    Type       *FetchType(Base &base, const char *type);
    Type       *FetchType(const char *base, const char *type);
    Property   *FetchProp(Type &type, const char *name);
    Property   *FetchProp(Base &base, const char *type, const char *name);
    Property   *FetchProp(const char *base, const char *type, const char *name);
    Parameters *FetchParams(Type &tc, const char *name);

    char *FullPath(const char *lib);
    Handle *GetHandle(const char *base, const char *type);
    void *GetFunction(Handle &h, const char *b, const char *t, const char *n);
    void *NewObject(Property &);
    void *FetchObj(Base &b, const char *type, const char *name);
    void *FetchNewObj(Base &b, const char *type, const char *name);
    void *FetchObj(Base &b, const char *type_name);
    void *FetchNewObj(Base &b, const char *type_name);
public:
    char *DBtype;
    
    DataBase(const char *DB_type="DataBase");
    virtual ~DataBase();
    DataBase_Error *ErrorHandler(DataBase_Error *handler=NULL);
    virtual char *HashName(const char *name);  
    int Read(const char *files);                // colon delimited list
    
    Parameters *FetchParams(const char *base,const char *type,const char *name);
    void *FetchObj(const char *base, const char *type, const char *name);
    void *FetchNewObj(const char *base, const char *type, const char *name);
    Units *FetchUnits(const char *type, const char *name);
    Units *FetchNewUnits(const char *type, const char *name);

    Parameters *FetchParams(const char *base,const char *type_name);
    void *FetchObj(const char *base, const char *type_name);
    void *FetchNewObj(const char *base, const char *type_name);
    Units *FetchUnits(const char *type_name);
    Units *FetchNewUnits(const char *type_name);

    int List(ostream &out, const char *property, int print=0);
    // print = -1, base:type::name
    //       =  0, base:type::name Units:utype::uname
    //       =  1, print parameters
    //       =  2, use then print parameters
    ostream &Summary(ostream &out) {List(out,"*:*::*",0); return out;}
    ostream &Print(ostream &out)   {List(out,"*:*::*",1); return out;}
    const char *BaseInfo(const char *base);
    const char *TypeInfo(const char *base, const char *type);
};

inline const char *DataBase::BaseInfo(const char *base)
{
    Base *b = FetchBase(base);
    return b ? b->info : NULL;
}

inline const char *DataBase::TypeInfo(const char *base, const char *type)
{
    Type *t = FetchType(base, type);
    return t ? t->info : NULL;
}

#endif // EOSLIB_DATA_BASE_H
