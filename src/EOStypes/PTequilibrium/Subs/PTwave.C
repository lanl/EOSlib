#include "PTequilibrium.h"


Hugoniot *PTequilibrium::shock(const HydroState &state)
{
    if( eos1==NULL || eos2==NULL || EOSstatus != EOSlib::good )
    {
        EOSerror->Log("PTequilibrium::shock", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }    
    if( lambda1 >= 1. )
        return eos1->shock(state);
    if( lambda2 >= 1. && de == 0.0 )
        return eos2->shock(state);
    return EOS::shock(state);    
}
//
//
Isentrope *PTequilibrium::isentrope(const HydroState &state)
{
    if( eos1==NULL || eos2==NULL || EOSstatus != EOSlib::good )
    {
        EOSerror->Log("PTequilibrium::isentrope", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }    
    if( lambda1 >= 1. )
        return eos1->isentrope(state);
    if( lambda2 >= 1. && de == 0.0 )
        return eos2->isentrope(state);
    return EOS::isentrope(state);
}
//
//
Isotherm  *PTequilibrium::isotherm(const HydroState &state)
{
    if( eos1==NULL || eos2==NULL || EOSstatus != EOSlib::good )
    {
        EOSerror->Log("PTequilibrium::isotherm", __FILE__, __LINE__, this,
	                   "status not good\n");
	    return NULL;
    }    
    if( lambda1 >= 1. )
        return eos1->isotherm(state);
    if( lambda2 >= 1.0 && de == 0.0 )
        return eos2->isotherm(state);
    return EOS::isotherm(state);
}
