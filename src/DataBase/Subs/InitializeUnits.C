#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}

#include "DataBase.h"

int Units::Initialize(const char *type, const char *name, DataBase &list)
{
    if( type == NULL || name == NULL )
        return 1;
    
    Property *u = list.FetchProp(*list.units, type, name);
    if( u == NULL )
        return 1;
    
    return Initialize(*u->params,list);
}


#define FUNC "Units::Initialize", __FILE__, __LINE__, NULL
int Units::Initialize(Parameters &u, DataBase &list)
{
    if( strcmp(u.base,"Units") )
    {
        DB_error->Log(FUNC, "type mismatch, %s\n", u.base);
        return 1;
    }
    
    
    const char *Values = NULL;
    const char *Names = NULL;
    
    ParamIterator Iter(u);
    const char *param;
    const char *value;
    
    while( Iter.Next(param,value) )
    {
        if( !strcmp(param, "Values") )
            Values = value;
        else if( !strcmp(param, "Names") )
            Names = value;
        else
        {
            DB_error->Log(FUNC, "unknown parameter, %s=%s\n", param, value);
            return 1;
        }
    }
    if( Values == NULL || Names == NULL )
    {
        DB_error->Log(FUNC, "values or names not specified\n");
        return 1;
    }

    
    delete [] utype; utype = Strdup(u.type);
    delete [] uname; uname = Strdup(u.name);
    delete values;   values = NULL;
    delete names;     names = NULL;
    
    if( (values=list.FetchParams(*list.unit_values, Values)) == NULL )
        DB_error->Log(FUNC, "error fetching Values %s\n", Values);
    
    if( (names=list.FetchParams(*list.unit_names, Names)) == NULL )
        DB_error->Log(FUNC, "error fetching Names %s\n", Names);
    names->Expand();

    if( values == NULL || names == NULL )
        return 1;
    
    values = values->Copy();
    names  =  names->Copy();
    return 0;
}

