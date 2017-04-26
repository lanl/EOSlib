#include "Porous.h"

#define FUNC "FrozenPorous::InitParams",__FILE__,__LINE__,this
int FrozenPorous::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    phi_f = B_f = NaN;
    calc.Variable("phi", &phi_f);
    calc.Variable("B",   &B_f);

    char *eos_name = NULL;
    char *phi_name = NULL;
    calc.Variable("EOS",   &eos_name);
    calc.Variable("PhiEq", &phi_name);
            
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
        
    if( eos_name == NULL || phi_name == NULL )
    {
        EOSerror->Log(FUNC,"solid or PhiEq not specified\n");
        return 1;
    }
    
    if( solid )
        deleteEOS(solid);
    if( (solid=FetchEOS(eos_name, *db)) == NULL )
    {
        EOSerror->Log(FUNC,"FetchEOS failed for %s\n", eos_name);
        return 1;
    }
    if( solid->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC,"solid->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }

    if( phi_eq )
        deletePhiEq(phi_eq);      
    if( (phi_eq=FetchPhiEq(phi_name, *db)) == NULL )
    {
        EOSerror->Log(FUNC,"FetchPhiEq failed for %s\n", phi_name);
        return 1;
    }
    if( PhiEqlib_mismatch(PhiEq_vers) )
    {
        EOSerror->Log(FUNC,"Library incompatiblity: EOS(%s), PhiEq(%s)\n",
              PhiEq_vers, PhiEqlib_vers);  
        return 1;
    }
    if( phi_eq->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC,"phi_eq->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }
    delete [] eos_name;
    delete [] phi_name;

    if( std::isnan(phi_f) )
        phi_f = phi_eq->phi_0();
    if( std::isnan(B_f) )
        B_f = phi_eq->energy(phi_f);
        
    // set reference state    
    if( std::isnan(e_ref) )
        e_ref = solid->e_ref + B_f;
    if( std::isnan(V_ref) )
        V_ref = solid->V_ref/phi_f;

    return 0;  
}    
