#include <EOS.h>
#include "Uniaxial.h"

#define FUNC "VonMisesElasticPlastic::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int VonMisesElasticPlastic::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    if( db == NULL )
    {
        EOSerror->Log(FUNC,"DataBase required\n");
        return 1;
    }
        
    G = Y = NaN;
	calcvar(G);
	calcvar(Y);

	char *eos_name = NULL;
    calc.Variable("eos",&eos_name);
			
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }

	if( eos_name == NULL )
	{
	    EOSerror->Log(FUNC,"eos not specified\n");
	    return 1;
	}
    if( !finite(G) || !finite(Y) )
    {
        EOSerror->Log(FUNC,"variable not set: G=%lf, T=%lf\n", G, Y);
        return 1;
    }	
	
	if( (eos=FetchEOS(eos_name,*db)) == NULL )
	{
	    EOSerror->Log(FUNC,"FetchEOS failed for %s\n", eos_name);
	    return 1;
	}
	if( units && eos->ConvertUnits(*units) )
	{
	    EOSerror->Log(FUNC,"ConvertUnits failed for units %s\n", p.units);
	    return 1;
	}
	
	if( std::isnan(V_ref) )
		V_ref = eos->V_ref;
	if( std::isnan(e_ref) )
		e_ref = eos->e_ref;
		
	if( G == 0.0 )
		Y = 0.0;
		
	if( Y == 0.0 )
		Vy_comp = Vy_exp = V_ref;
	else if( std::isnan(Y) )
	{
		Vy_comp = 0.0;
		Vy_exp  = HUGE;
	}
	else
	{
		double eps_y = 0.5*Y/G;
		Vy_comp = V_ref*exp(-eps_y);
		Vy_exp  = V_ref*exp( eps_y);
	}

    delete[] eos_name;
	return 0;
}
