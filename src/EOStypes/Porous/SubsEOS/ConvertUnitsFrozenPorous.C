#include "Porous.h"

#define FUNC "EqPorous::ConvertParams", __FILE__, __LINE__, this
int FrozenPorous::ConvertParams(Convert &convert)
{
    if( solid == NULL || phi_eq == NULL )
        return 1;

    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    if( !finite(B_f *= convert.factor("e")) )
    {
        EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n" );
        return 1;
    }
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
