#include <iostream>
using namespace std;
#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
#include "DataBase.h"

int DataBase::use = 5;
Handle *DataBase::main = NULL;
//
int DataBase::init = 0;
DataBase_Error *DataBase_cerr            = NULL;
DataBase_Error *DataBase::error_default  = NULL;
// note: global variables set if linked with lib.a
//       but not with lib.so;
//       instead set by DataBase::Init(), called from DataBase::DataBase()
void DataBase::Init()
{
    if( init )
        return;
    DataBase_cerr = new DataBase_Error(cerr);
    error_default  = DataBase_cerr;
    init = 1;
}
extern "C" { // dynamic link for database
  void DataBase_Init()
  {
      DataBase::Init();
  }
}

const char *EOSlib::Status(EOSlib::STATUS status)
{
    switch(status)
    {
        case EOSlib::good:
            return "good";
        case EOSlib::fail:
            return "fail";
        case EOSlib::bad:
            return "bad";
        case EOSlib::abort:
            return "abort";
        default:
            return "?";
    }
}

//
//
//
int TypeName(const char *str, const char *&type, const char *&name)
{ // str = type::name
    type = name = NULL;
    if( str == NULL )
        return 1;
    int n=0;
    for( n=0; str[n] != '\0'; n++ )
    {
        if( str[n] == ':' )
        {
            if( n==0 || str[n+1] != ':' || str[n+2]=='\0' )
                return 1;
            name = Strdup(str+n+2);
            char *ttype = new char[n+1];
            ttype[n]='\0';
            memcpy(ttype,str,n);
            type = ttype;
            return 0;
        }
    }
    return 1;  
}

DataBase::DataBase(const char *DB_type) : vars(NULL), DBtype(Strdup(DB_type))
{
    if( init == 0 )
        Init();
    error = error_default;

    Base *base;
    Type *type;
    char *hname;

    globals = &GetBase("");
    units   = &GetBase("Units");
    units->Initialize = &Units_Initialize;
    units->Duplicate  = &Units_Duplicate;
    units->Destructor = &Units_Destructor;
    unit_values  = &GetType(*globals,"UnitsValues");
    unit_names   = &GetType(*globals,"UnitsNames");
    shared_libs  = &GetType(*globals,"SharedLibs");
    database     = &GetType(*globals,"DataBase");

    Property &prop = GetProp(*database,"Environment");
    prop.params = new Parameters;
    prop.params->base  = Strdup("");
    prop.params->type  = Strdup("DataBase");
    prop.params->name  = Strdup("Environment");
    prop.params->units = Strdup("");
    prop.params->Append("PWD",getenv("PWD"));
}

DataBase::~DataBase()
{
    delete [] DBtype;
}

DataBase_Error *DataBase::ErrorHandler(DataBase_Error *handler)
{
    DataBase_Error *old = error;
    if( handler ) error = handler;
        return old;
}

// Default HashName
// Alternative, for example, could convert to lower case
char *DataBase::HashName(const char *name)
{
    return Strdup(name);
}

Base &DataBase::GetBase(const char *base)
{
    char *n  = HashName(base);
    Base &bc = bases[n];
    if( bc.base == NULL ) 
        bc.base = Strdup(base);
    delete [] n;
    return bc;      
}

Type &DataBase::GetType(Base &bc, const char *type)
{
    char *n  = HashName(type);
    Type &tc = bc.types[n];
    if( tc.type == NULL )
    {
        tc.type = Strdup(type);
        tc.base = &bc;
    }
    delete [] n;
    return tc;
}

Property &DataBase::GetProp(Type &type, const char *name)
{
    char *n = HashName(name);
    Property &prop= type.props[n];
    if( prop.type == NULL )
        prop.type = &type;
    delete [] n;
    return prop;
}

//

Base *DataBase::FetchBase(const char *base)
{
    if( base == NULL || base[0] == '\0' )
        return globals;
    char *b = HashName(base); 
    Base *ans = bases.Defined(b) ? &bases.LastElement() : NULL;
    delete [] b;
    return ans;      
}

Type *DataBase::FetchType(Base &base, const char *type)
{
    char *t = HashName(type);
    Type *ans = base.types.Defined(t) ? &base.types.LastElement() : NULL;
    delete [] t;
    return ans;      
}

Type *DataBase::FetchType(const char *base, const char *type)
{
    Base *b = FetchBase(base);
    return b ? FetchType(*b,type) : NULL;
}

Property *DataBase::FetchProp(Type &type,const char *name)
{
    char *n = HashName(name);
    Property *ans = type.props.Defined(n) ?  &type.props.LastElement() : NULL;
    delete [] n;
    return ans;
}

Property *DataBase::FetchProp(Base &base, const char *type,const char *name)
{
    Type *t = FetchType(base,type);
    return t ? FetchProp(*t,name) : NULL;
}

Property *DataBase::FetchProp(const char *base, const char *type,
                              const char *name)
{ 
    Type *t = FetchType(base,type);
    return t ? FetchProp(*t,name) : NULL;
}

Parameters *DataBase::FetchParams(Type &t, const char *name)
{
    Property *prop = FetchProp(t,name);
    if( prop == NULL || Use(t.props, *prop->params) )
        return NULL;

    return prop->params;
}

//

Parameters *DataBase::FetchParams(const char *base, const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name, type,name) )
        return NULL;
    Parameters *params = FetchParams(base,type,name);
    delete [] name;
    delete [] type;
    return params;
}

Parameters *DataBase::FetchParams(const char *b, const char *t,const char *n)
{
    Type *tc = FetchType(b,t);
    return tc ? FetchParams(*tc, n) : NULL;
}

Units *DataBase::FetchUnits(const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name,type,name) )
        return NULL;
    Units *u = FetchUnits(type, name);
    delete [] type;
    delete [] name;
    return u; 
}

Units *DataBase::FetchNewUnits(const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name,type,name) )
        return NULL;
    Units *u = FetchNewUnits(type, name);
    delete [] type;
    delete [] name;
    return u; 
}

Units *DataBase::FetchUnits(const char *type, const char *name)
{
    Property *prop = FetchProp(*units,type, name);
    if( prop == NULL )
        return NULL;
    if( prop->obj  == NULL )
    {
        Units *u = new Units;
        if( u->Initialize(*prop->params, *this) )
        {
            delete u;
            return NULL;
        }
        prop->obj = (void *)u;
    }
    return ((Units *)(prop->obj))->Duplicate();
}

Units *DataBase::FetchNewUnits(const char *type, const char *name)
{
    Units *u = new Units;
    if( u->Initialize(type, name, *this) )
    {
        delete u;
        u = NULL;
    }
    return u;    
}
