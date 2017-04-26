#include "Porous.h"

#define FUNC "EqPorous::ConvertParams", __FILE__, __LINE__, this
int EqPorous::ConvertParams(Convert &convert)
{
    if( solid == NULL || phi_eq == NULL )
        return 1;
    
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    double s_e;
    double s_V;
    if( !finite(s_e = convert.factor("e"))
         || !finite(s_V = convert.factor("V")) )
    {
        EOSerror->Log(FUNC, "failed\n" );
        return 1;
    }
    B_max *= s_e;
    dBdphi_max *= s_e;
    
    if( solid->ConvertUnits(convert) )
    {
        EOSerror->Log(FUNC, "solid->ConvertUnits failed\n" );
        return 1;
    }
    if( phi_eq->ConvertUnits(convert) )
    {
        EOSerror->Log(FUNC, "phi_eq->ConvertUnits failed\n" );
        return 1;
    }
    
    return 0;
}

