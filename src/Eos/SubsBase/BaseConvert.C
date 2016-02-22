#include "EOSbase.h"

int EOSbase::ConvertParams(Convert &)
{
    // To be supplied by derived class
    return 1;
}

int EOSbase::ConvertUnits(Units &To)
{
    if( units == NULL )
        return 1;
    if( units->Match(To) )
        return 0;
    
    Convert convert(*units, To);
    return ConvertUnits(convert);
}

#define FUNC "EOSbase::ConvertUnits", __FILE__, __LINE__, this
int EOSbase::ConvertUnits(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log(FUNC, "status not good\n");
        return 1;
    }
    if( convert )
    {
        EOSerror->Log(FUNC, "convert not good\n");
        return 1;
    }
    if( convert.MatchTo(units) )
        return 0;
    if( !convert.MatchFrom(units) )
    {
        EOSerror->Log(FUNC, "units mismatch\n");
        return 1;
    }

    int status = ConvertParams(convert);
    if( status > 0 )
    {
        EOSstatus = EOSlib::fail;
        EOSerror->Log(FUNC, "failed, unchanged\n");
        return 1;
    }
    if( status < 0 )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Log(FUNC, "failed, fatal\n");
        return -1;
    }
    
    deleteUnits(units);
    units = convert.To();
    return 0;   
}


int EOSbase::ConvertUnits(const char *type_name, DataBase &db)
{
    if( type_name == NULL )
        return 1;
    
    Units *u = db.FetchUnits(type_name);
    if( u == NULL )
        return 1;            
    int ret = ConvertUnits(*u);
    deleteUnits(u);
    return ret;
}

int EOSbase::ConvertUnits(const char *u_type,const char *u_name, DataBase &db)
{
    if( u_type == NULL || u_name == NULL )
        return 1;
    
    Units *u = db.FetchUnits(u_type, u_name);
    if( u == NULL )
        return 1;            
    int ret = ConvertUnits(*u);
    deleteUnits(u);
    return ret;
}
