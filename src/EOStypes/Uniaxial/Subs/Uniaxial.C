#include <EOS.h>
#include "Uniaxial.h"

UniaxialEOS::~UniaxialEOS()
{
	deleteEOS(eos);
}

int UniaxialEOS::ConvertParams(Convert &convert)
{
    if( eos == NULL )
    {
	    EOSerror->Log("Uniaxial::ConvertUnits",__FILE__, __LINE__, this,
                       "eos is NULL\n");
	    return 1;
    }
	if( eos->ConvertUnits(convert) )
	{
	    EOSerror->Log("Uniaxial::ConvertUnits",__FILE__, __LINE__, this,
                "eos->ConvertUnits failed\n");
	    return 1;
	}
    return 0;    
}

double UniaxialEOS::P(double V, double e)
{
	e -= ShearEnergy(V,e);
	return eos->P(V,e) + stress_dev(V,e);
}

double UniaxialEOS::T(double V, double e)
{
	e -= ShearEnergy(V,e);
	return eos->T(V,e);
}

double UniaxialEOS::S(double V, double e)
{
	e -= ShearEnergy(V,e);
	return eos->S(V,e);
}


double UniaxialEOS::c2(double V, double e)
{
	e -= ShearEnergy(V,e);
	double c2_el = eos->c2(V,e);
	return IsElastic(V,e) ? c2_el+(4./3.)*V*ShearModulus(V,e) : c2_el;
}

double UniaxialEOS::Gamma(double V, double e)
{
// add correction for 
	e -= ShearEnergy(V,e);
	return eos->Gamma(V,e);
}

double UniaxialEOS::CV(double V, double e)
{
	e -= ShearEnergy(V,e);
	return eos->CV(V,e);
}

int UniaxialEOS::NotInDomain(double V, double e)
{
	return eos->NotInDomain(V,e);
}

