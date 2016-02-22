#include "Porous.h"

#define FUNC "EqPorous::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int EqPorous::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
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
        
    // set reference state    
    if( isnan(e_ref) )
        e_ref = solid->e_ref;
    if( isnan(V_ref) )
        V_ref =solid->V_ref/phi_eq->phi_0();

    return InitPhi();  
}
