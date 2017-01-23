#include "EOSbase.h"

#define FUNC "EOSbase::Initialize",__FILE__,__LINE__,this

int EOSbase::Initialize(const char *pname, DataBase &db, const char *uname)
{
    if( base == NULL || type == NULL )
    {
        EOSstatus = EOSlib::abort;
        EOSerror->Log(FUNC,"failed, base or type is NULL\n");
        return 1;
    }
    if( pname == NULL )
    {
        EOSstatus = EOSlib::abort;
        EOSerror->Log(FUNC,"failed, parameter name is NULL\n");
        return 1;
    }
    Parameters *params = db.FetchParams(base, type, pname);
    if( params == NULL )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Log(FUNC,"FetchParams failed for %s:%s::%s\n", base, type, pname);
        return 1;
    }    
    Units *u = NULL;
    if( uname )
    {
        u = db.FetchUnits(uname);
        if( u == NULL )
        {
            EOSstatus = EOSlib::bad;
            EOSerror->Log(FUNC,"FetchUnits failed for %s\n", uname);
            return 1;
        }
    }
    return Initialize(*params, &db, u);
}

#undef FUNC
#define FUNC "EOSbase::Initialize",__FILE__,__LINE__,this
int EOSbase::Initialize(Parameters &p, DataBase *db, Units *to)
{
    EOSstatus = EOSlib::bad;
    if( strcmp(p.base, base) )
    {
        EOSerror->Log(FUNC,"base mismatch: %s %s\n",p.base, base);
        return 1;
    }
    if( strcmp(p.type, type) )
    {
        EOSerror->Log(FUNC,"type mismatch: %s %s\n",p.type, type);
        return 1;
    }
            
    Rename(p.name);
    if( units )
        deleteUnits(units);
    if( p.units != NULL )
    {
        if( to && to->Match(p.units) )
        {
            units = to->Duplicate();  
        }
        else if( db )
        {
            units = db->FetchUnits(p.units);
            if( units == NULL )
            {
                EOSerror->Log(FUNC,"FetchUnits failed for %s\n",p.units);
                return 1;
            }
        }
        else
        {
            EOSerror->Log(FUNC,"need DataBase to set units\n");
            return 1;
        }
    }
    
    Calc calc;
    if( InitBase(p, calc, db) )
    {
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }
    
    EOSstatus = EOSlib::good;
    return (to == NULL) ? 0 : ConvertUnits(*to);
}
