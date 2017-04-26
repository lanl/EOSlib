#include "ExtEOS.h"

ExtEOS::ExtEOS(const char *eos_type) : EOS(eos_type), z_ref(NULL), frozen(1)
{
    ztmp = NULL;
    n = 0;
}

ExtEOS::ExtEOS(const ExtEOS &e_eos) : EOS(e_eos)
{
    frozen = e_eos.frozen;
    if( e_eos.z_ref )
    {
        z_ref = e_eos.z_ref->Copy();
        if( z_ref == NULL )
        {
            EOSstatus = EOSlib::abort;
            EOSerror->Log("ExtEOS::ExtEOS",__FILE__,__LINE__,this, 
                          "z_ref->Copy returned NULL\n" );
            return;
        }
        n = e_eos.n;
        ztmp = (n>0) ? new double[n] : NULL;
        int i;
        for( i=0; i<n; i++ )
            ztmp[i] = e_eos.ztmp[i];
    }
    else
    {
        z_ref = NULL;
        ztmp  = NULL;        
        n     = 0;
    }
}

ExtEOS::~ExtEOS()
{
    delete z_ref;
    delete ztmp;
}

int ExtEOS::Replace(const double *z)
{
    int i;
    if( z )
    {
        if( z_ref == NULL )
            return 1;
        for( i=0; i<n; i++ )
            (*z_ref)[i] = z[i];
    }
    if( frozen )
    {
        for( i=0; i<n; i++ )
            ztmp[i] = (*z_ref)[i];
    }
    return 0;
}

EOS *ExtEOS::CopyEOS(const double *z)
{
    return Copy(z);
}
ExtEOS *ExtEOS::Copy(const double *z)
{
    ExtEOS *e_eos = Copy();
    if( e_eos == NULL )
        return NULL;
    if( e_eos->Replace(z) )
    {
        deleteExtEOS(e_eos);
        return NULL;
    }     
    return e_eos;
}

void ExtEOS::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    out << "\tfrozen = " << frozen << "\n";
    if( z_ref )
        z_ref->PrintParams(out);
}

int ExtEOS::ConvertParams(Convert &convert)
{
    if( z_ref && z_ref->ConvertParams(convert) )
        return 1;
    return EOS::ConvertParams(convert);
}

int ExtEOS::PreInit(Calc &)
{
    return 1;
}

int ExtEOS::PostInit(Calc &, DataBase *)
{
    return 1;
}
#define FUNC "ExtEOS::InitParams",__FILE__,__LINE__,this
int ExtEOS::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    EOSstatus = EOSlib::abort;
    frozen = 1;
    n = 0;
    delete [] ztmp; ztmp = NULL;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    calc.Variable( "frozen",  &frozen );
    if( z_ref && z_ref->InitParams(calc) )
    {
        EOSerror->Log(FUNC, "z_ref->InitParams failed\n" );
        return 1;        
    }
    if( PreInit(calc) )     // derived class
    {
        EOSerror->Log(FUNC, "PreInit failed\n" );
        return 1;        
    }
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    EOSstatus = EOSlib::good;
	if( PostInit(calc,db) ) // derived class
    {
        EOSstatus = EOSlib::abort;
        return 1;
    }
    if( z_ref )
    {
        n = z_ref->N();
        ztmp = new double[n];
        int i;
        for( i=0; i<n; i++ )
            ztmp[i] = (*z_ref)(i);
    }
    return 0;
}
