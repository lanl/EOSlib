#include "ExtEOSstate.h"

ExtEOSstate::ExtEOSstate(ExtEOS &mat, const double *zs)
                : xeos(NULL), z(NULL)
{
     V = mat.V_ref;
     e = mat.e_ref;
     u = 0.0;
    us = EOS::NaN;
    Init(mat, zs);
}

ExtEOSstate::ExtEOSstate(ExtEOS &mat, const HydroState &state, const double *zs)
                : xeos(NULL), z(NULL)
{
     V = state.V;
     e = state.e;
     u = state.u;
    us = EOS::NaN;
    Init(mat, zs);
}

ExtEOSstate::ExtEOSstate(ExtEOS &mat, const WaveState &state, const double *zs)
                : xeos(NULL), z(NULL)
{
     V = state.V;
     e = state.e;
     u = state.u;
    us = state.us;
    Init(mat, zs);
}

void ExtEOSstate::Init(ExtEOS &mat, const double *zs)
{
    delete [] z;           z = NULL;
    deleteExtEOS(xeos); xeos = NULL;
    xeos = mat.Duplicate();
    int n = xeos->N();
    if( n > 0 )
        z = new double[n];
    Evaluate(zs);
}

void ExtEOSstate::Evaluate(const double *zs)
{
    if( xeos == NULL )
        return;
    int n = xeos->N();
    if( n > 0 )
    {
        if( zs == NULL )
            zs = xeos->z_f(V,e);
        if( zs != z )
        {
            int i;
            for( i=0; i<n; i++ )
                z[i] = zs[i];
        }
    }
    if( n <= 0 || xeos->IsFrozen()==0 )
    {
        P = xeos->P(V,e);
        T = xeos->T(V,e);
        S = xeos->S(V,e);
        c = xeos->c(V,e);
    }
    else
    {
        P = xeos->P(V,e,z);
        T = xeos->T(V,e,z);
        S = xeos->S(V,e,z);
        c = xeos->c(V,e,z);
    }
}
    
ExtEOSstate::~ExtEOSstate()
{
    deleteExtEOS(xeos);
    delete [] z;
}

void ExtEOSstate::operator=(const ExtEOSstate &state)
{
     V = state.V;
     e = state.e;
     u = state.u;
    us = state.us;
    if( state.xeos )
        Init( *state.xeos, state.z);
    else
    {
        delete [] z;           z = NULL;
        deleteExtEOS(xeos); xeos = NULL;        
    }
}

void ExtEOSstate::operator=(const HydroState &state)
{
     V = state.V;
     e = state.e;
     u = state.u;
    us = EOS::NaN;
    Evaluate();
}

void ExtEOSstate::operator=(const WaveState &state)
{
     V = state.V;
     e = state.e;
     u = state.u;
    us = state.us;
    Evaluate();
}

void ExtEOSstate::Load(Calc &calc)
{
    calc.Variable( "V",&V);
    calc.Variable( "e",&e);
    calc.Variable( "u",&u);
    calc.Variable( "P",&P);
    calc.Variable( "T",&T);
    calc.Variable( "S",&S);
    calc.Variable( "c",&c);
    calc.Variable("us",&us);
    CalcVar *rho = new CVpinvdouble(&V);
    calc.Variable("rho", rho);    
    IDOF *idof;
    if( z && xeos && (idof=xeos->Zref()) )
        idof->Load(calc, z);
}

ostream &operator<<(ostream &out, ExtEOSstate &state)
{
    if( state.xeos == NULL )
        return out;

    out << "xeos = ";
    if( strcmp("Hydro",state.xeos->Type()) )
        out << state.xeos->Type() << "::";
    out << state.xeos->Name() << "\n";
    
    out << "units = ";
    const Units *u = state.xeos->UseUnits();
    if( u )
        out << u->Type() << "::" << u->Name() << "\n";
    else
        out << "NULL\n";
        
    out << "  V = " << state.V << "\n"
        << "  e = " << state.e << "\n"
        << "  u = " << state.u << "\n"
        << "  P = " << state.P << "\n"
        << "  T = " << state.T << "\n"
        << "  S = " << state.S << "\n"
        << "  c = " << state.c << "\n";
    if( finite(state.us) )
        out << " us = " << state.us << "\n";

    if( state.z )
    {
        IDOF *idof = state.xeos->Zref();
        if( idof )
        {
            idof->Print(out, state.z);
            out << "\tNidof = " << idof->N() << "\n";
            if( idof->var_name )
            {
                out << "\tvar_name = " << idof->var_name[0];
                int i;
                for( i=1; i<idof->N(); i++ )
                    out << "," << idof->var_name[i];
                out << "\n";
            }
        }
    }
    else
    {
        out << "\tNidof = 0\n";
    }
    return out;
}
