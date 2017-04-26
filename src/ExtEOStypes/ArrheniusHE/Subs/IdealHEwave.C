#include "IdealHE.h"
#include "IdealHE_Hugoniot.h"
#include "IdealHE_Isentrope.h"
#include "IdealHE_Isotherm.h"

Isentrope *IdealHE::isentrope(const HydroState &state)
{
	Isentrope *S = new Isentrope_IdealHE(*this);
	int status;	
	if( S==NULL || (status=S->Initialize(state)) )
	{
	     EOSerror->Log("IdealHE::isentrope", __FILE__, __LINE__, this,
	                    "Initialize failed with status %d\n", status);
	     delete S;
	     S = NULL;
	}
	return S;
}

Hugoniot *IdealHE::shock(const HydroState &state)
{
	Hugoniot *H = new Hugoniot_IdealHE(*this);
	int status;
	if( H==NULL || (status=H->Initialize(state)) )
	{
	    EOSerror->Log("IdealHE::shock", __FILE__, __LINE__, this,
	                  "Initialize failed with status %d\n", status);
	    delete H;
	    H = NULL;
	}
	return H;
}

Isotherm *IdealHE::isotherm(const HydroState &state)
{
	Isotherm *T = new Isotherm_IdealHE(*this);
	int status;	
	if( T==NULL || (status=T->Initialize(state)) )
	{
	    EOSerror->Log("IdealHE::isotherm", __FILE__, __LINE__, this,
	                   "Initialize failed with status %d\n", status);
	    delete T;
	    T = NULL;
	}
	return T;
}

Detonation *IdealHE::detonation(const HydroState &state, double P0)
{
	EOS *eos = this->products();
    IdealHE *Products = static_cast<IdealHE *>(eos);
    Detonation *H = new Detonation_IdealHE(*Products);
    deleteEOS(eos);
	int status;
	if( H==NULL || (status=H->Initialize(state,P0)) )
	{
	    EOSerror->Log("IdealHE::detonation", __FILE__, __LINE__, this,
	                  "Initialize failed with status %d\n", status);
	    delete H;
	    H = NULL;
	}
	return H;
}

Deflagration *IdealHE::deflagration(const HydroState &state, double P0)
{
	Deflagration *H = new Deflagration_IdealHE(*this);
	int status ;
	if( H==NULL || (status=H->Initialize(state,0.0)) )
	{
	    EOSerror->Log("IdealHE::deflagration", __FILE__, __LINE__, this,
	                  "Initialize failed with status %d\n", status);
	    delete H;
	    H = NULL;
	}
	return H;
}
