#include "ExtEOS.h"

Isentrope *ExtEOS::isentrope(const HydroState &state, const double *z)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("ExtEOS::isentrope", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    EOS *eos = CopyEOS(z);
    if( eos == NULL )
    {
        EOSerror->Log("ExtEOS::isentrope", __FILE__, __LINE__, this,
	                   "Copy failed\n");
	    return NULL;
    }
    Isentrope *I = eos->isentrope(state);
    deleteEOS(eos);
    return I;  
}

Hugoniot *ExtEOS::shock(const HydroState &state, const double *z)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("ExtEOS::shock", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    EOS *eos = CopyEOS(z);
    if( eos == NULL )
    {
        EOSerror->Log("ExtEOS::shock", __FILE__, __LINE__, this,
	                   "Copy failed\n");
	    return NULL;
    }
    Hugoniot *hug = eos->shock(state);
    deleteEOS(eos);
    return hug;  
}

Detonation *ExtEOS::detonation(const HydroState &state, double P0,
                                                        const double *z)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("ExtEOS::detonation", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    EOS *eos = CopyEOS(z);
    if( eos == NULL )
    {
        EOSerror->Log("ExtEOS::detonation", __FILE__, __LINE__, this,
	                   "Copy failed\n");
	    return NULL;
    }
    Detonation *hug = eos->detonation(state,P0);
    deleteEOS(eos);
    return hug;  
}
Deflagration *ExtEOS::deflagration(const HydroState &state, double P0,
                                                        const double *z)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("ExtEOS::deflagration", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    EOS *eos = CopyEOS(z);
    if( eos == NULL )
    {
        EOSerror->Log("ExtEOS::deflagration", __FILE__, __LINE__, this,
	                   "Copy failed\n");
	    return NULL;
    }
    Deflagration *hug = eos->deflagration(state,P0);
    deleteEOS(eos);
    return hug;  
}

Isotherm *ExtEOS::isotherm(const HydroState &state, const double *z)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("ExtEOS::isotherm", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    EOS *eos = CopyEOS(z);
    if( eos == NULL )
    {
        EOSerror->Log("ExtEOS::isotherm", __FILE__, __LINE__, this,
	                   "Copy failed\n");
	    return NULL;
    }
    Isotherm *I = eos->isotherm(state);
    deleteEOS(eos);
    return I;  
}
