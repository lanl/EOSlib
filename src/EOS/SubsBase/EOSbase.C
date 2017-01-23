#include "LocalMath.h"
#include "EOSbase.h"
#include <iostream>
using namespace std;
#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}

int EOSbase::initbase                 = 0;       // 1;
int EOSbase::max_ref_count            = 1000;
EOSbase::Error *EOScerr               = NULL;    // new EOSbase::Error(cerr);
EOSbase::Error *EOSbase::ErrorDefault = NULL;    // EOS_cerr;
// note: global variables set if linked with lib.a
//       but not with lib.so;
//       instead set by EOS::Init(), called from EOS::EOS()
//

double EOSbase::NaN = ::NaN;
//
void EOSbase::Initbase()
{
    if( initbase )
        return;
    EOScerr = new Error(cerr);
    
    NaN = ::NaN;
    ErrorDefault = EOScerr;
    initbase = 1;
}
//
//
//
int deleteEOSbase(EOSbase *eos, const char *base)
{
    int status;
    if( eos == NULL )
        status = 0;
    else if( eos->ref_count == 1 )
    {
        delete eos;
        status = 0;
    }
    else
    {
        status = --(eos->ref_count);
        if( status > EOSbase::max_ref_count || status < 0 )
        {
            eos->EOSstatus = EOSlib::abort;
            if( base == NULL )
                base = "?";
            char typ[15];
            if( eos->type )
            {
                int i;
                for( i=0; i<15; i++ )
                {
                    typ[i] = eos->type[i];
                    if( typ[i] == '\0' )
                        break;
                }
                typ[14] = '\0';
            }
            else
            {
              typ[0] = '?';
              typ[1] = '\0';
            }
            char nm[15];
            if( eos->name )
            {
                int i;
                for( i=0; i<15; i++ )
                {
                    nm[i] = eos->name[i];
                    if( nm[i] == '\0' )
                        break;
                }
                nm[14] = '\0';
            }
            else
            {
              nm[0] = '?';
              nm[1] = '\0';
            }
            EOSbase::ErrorDefault->Abort("deleteEOSbase", __FILE__, __LINE__,
                 NULL, "ref_count=%d, Deallocation error for %s:%s::%s\n",
                 status,base,typ,nm);
            status = -1;
        }
        /************
        else
        { // debug
            EOSbase::ErrorDefault->Log("deleteEOSbase", __FILE__, __LINE__,
                 eos, "debug: ref_count=%d", eos->ref_count);
        }
        *************/
    }
    return status;
}
EOSbase *EOSbase::Duplicate()
{
    if( ref_count > max_ref_count )
    {
        EOSbase::ErrorDefault->Abort("EOSbase::Duplicate", __FILE__, __LINE__,
                 this, "ref_count > max_ref_count");
    }
    ref_count++;
    return this;
}

EOSbase::EOSbase(const char *eos_base, const char *eos_type)
                : name(NULL), units(NULL), ref_count(1), verbose(0)
{
    if( initbase == 0 )
        Initbase();

    base = Strdup(eos_base);   
    type = Strdup(eos_type);   
    EOSstatus = base ? EOSlib::bad : EOSlib::abort;
    EOSerror  = ErrorDefault;
}

EOSbase::EOSbase(const EOSbase &b) : ref_count(1)
{
    verbose = b.verbose;
    base = Strdup(b.base);
    type = Strdup(b.type);
    name = Strdup(b.name);
    units = b.units ? b.units->Duplicate() : NULL;
    EOSstatus = b.EOSstatus;
    EOSerror  = b.EOSerror;
}

EOSbase::~EOSbase()
{
    EOSstatus = EOSlib::abort;
    //ErrorDefault->Log("EOSbase::~EOSbase", __FILE__, __LINE__,
    //                        this, "debug: ref_count %d",ref_count-1);
    if( --ref_count )
    {
        ErrorDefault->Abort("EOSbase::~EOSbase", __FILE__, __LINE__,
                            this, "ref_count %d !=0",ref_count);
        return;
    }
    if( base )
        delete [] base;
    if( type )
        delete [] type;
    if( name )
        delete [] name;
    // base = type = name = NULL;
    deleteUnits(units);
}

int EOSbase::Copy(const EOSbase &b, int check)
{
    if( check && (strcmp(b.base,base) || strcmp(b.type,type)) )
        return 1;
    delete [] name;
    name = Strdup(b.name);
    deleteUnits(units);
    units = b.units ? b.units->Duplicate() : NULL;
    EOSstatus = b.EOSstatus;
    if( EOSerror == NULL )
        EOSerror  = b.EOSerror;
    verbose = b.verbose;
    return 0;
}

EOSbase::Error *EOSbase::ErrorHandler(Error *handler)
{
    Error *old = EOSerror;
    if( handler )
        EOSerror = handler;
    return old;
}


void EOSbase::Rename(const char *mat_name)
{
    if( name )
        delete [] name;
    name = Strdup(mat_name);
}
