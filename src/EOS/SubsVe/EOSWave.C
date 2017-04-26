#include "EOS.h"
#include "Isentrope_ODE.h"
#include "Isotherm_ODE.h"
#include "Hugoniot_gen.h"
#include "Detonation_gen.h"
#include "Deflagration_gen.h"


Isentrope *EOS::isentrope(const HydroState &state)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("EOS::isentrope", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    Isentrope *S = new Isentrope_ODE(*this);
    if( S==NULL || S->Initialize(state) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("EOS::isentrope", __FILE__, __LINE__, this,
                       "Initialize failed");
    }
    return S;
}

Hugoniot *EOS::shock(const HydroState &state)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("EOS::shock", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    Hugoniot *S = new Hugoniot_generic(*this);
    if( S==NULL || S->Initialize(state) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("EOS::shock", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return S;
}

Detonation *EOS::detonation(const HydroState &state, double p0)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("EOS::detonation", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    Detonation *S = new Detonation_gen(*this);
    if( S==NULL || S->Initialize(state,p0) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("EOS::detonation", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return S;
}

Deflagration *EOS::deflagration(const HydroState &state, double p0)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("EOS::deflagration", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    Deflagration *S = new Deflagration_gen(*this);
    if( S==NULL || S->Initialize(state,p0) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("EOS::deflagration", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return S;
}


Isotherm *EOS::isotherm(const HydroState &state)
{
    if( EOSstatus != EOSlib::good )
    {
        EOSerror->Log("EOS::isotherm", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }
    Isotherm *T = new Isotherm_ODE(*this);
    if( T==NULL || T->Initialize(state) )
    {
        delete T;
        T = NULL;
        EOSerror->Log("EOS::isotherm", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return T;
}
