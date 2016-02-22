#include "DataBase.h"

// Recursive upto count levels
#define FUNC "DataBase::Use", __FILE__, __LINE__, this
int DataBase::Use(NAMEARRAY<Property> &prop, Parameters &params, int count)
{
    if( params.use <= 0 )
        return params.use;
        
    if( count-- <= 0 )
    {
        DB_error->Log(FUNC, "name=%s, type=%s, too many levels "
                  "(possible loop)\n", params.name, params.type);
        return -1;
    }
    
    int status = 0;
    Parameters Old = Parameters(params);
    ParamIterator iter(Old);
    
    const char *p;
    const char *v;    
    while( iter.Next(p,v) )
    {
        params.Append(p,v);
        if( !strcmp(p, USE) )
        {
            if( !prop.Defined(v) || prop.LastElement().params == NULL )
            {
                DB_error->Log(FUNC, "name=%s, type=%s, use=%s not found\n",
                          params.name, params.type, v);
                continue;
            }
            Parameters &Uparams = *prop.LastElement().params;
            if( UnitsMismatch(Uparams, params) )
                DB_error->Log(FUNC,"Warning units mismatch for name=%s, "
                   "type=%s, use=%s,\n", params.name,params.type,Uparams.name);
            
            int use_p = Use(prop, Uparams, count);
            if( use_p >= 0 )
            {
                params.DeleteLast();
                if( params.Append(Uparams) )
                {
                    DB_error->Abort(FUNC, "memory allocation failed");
                    break; 
                }
            }
            else
                status = -1;
        }
    }
    
    if( params.use )
        DB_error->Log(FUNC, "Error use = %d, for %s:%s::%s\n",
                  params.use, params.base,params.type, params.name);
    
    return status == 0 ? params.use : -1;
}

