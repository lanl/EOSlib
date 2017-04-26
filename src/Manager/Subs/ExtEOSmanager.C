#include "ExtEOSmanager.h"
#include "Hydro.h"

ExtEOS_Array::~ExtEOS_Array()
{
    int i;
    for( i=0; i<n; i++)
    {
	     delete [] names[i];
	     ExtEOS *eptr = static_cast<ExtEOS *>(pointers[i]);
	     deleteExtEOS(eptr);
    }
    n = 0;
}

ExtEOSmanager::~ExtEOSmanager()
{
    // Null
}


int ExtEOSmanager::index_eos(const char *name, const char *unit)
{
    int i = EOSmanager::index_eos(name,unit);
    if( i < ex.N() )
        return i;
    EOS *ei = e[i];
    ExtEOS *exi = dynamic_cast<ExtEOS*>(ei);
    if( exi )
        exi = exi->Duplicate();
    else
        exi = new Hydro(*ei); 
    ex.Add( name, exi);
    return i;
}
