#include "Hydro.h"

ExtEOS *FetchExtEOS(const char *type, const char *name, DataBase &db)
{
    EOS *eos = static_cast<EOS*>(db.FetchObj("EOS",type,name));
    if( eos == NULL )
        return NULL;
    ExtEOS *ext = dynamic_cast<ExtEOS*>(eos);
    if( ext == NULL )
    {
        ext = new Hydro(*eos);
        deleteEOS(eos);
    }
    return ext;
}

ExtEOS *FetchNewExtEOS(const char *type, const char *name, DataBase &db)
{
    EOS *eos = static_cast<EOS*>(db.FetchNewObj("EOS",type,name));
    if( eos == NULL )
        return NULL;
    ExtEOS *ext = dynamic_cast<ExtEOS*>(eos);
    if( ext == NULL )
    {
        ext = new Hydro(*eos);
        deleteEOS(eos);
    }
    return ext;
}

ExtEOS *FetchExtEOS(const char *type_name, DataBase &db)
{
    EOS *eos = static_cast<EOS*>(db.FetchObj("EOS",type_name));
    if( eos == NULL )
        return NULL;
    ExtEOS *ext = dynamic_cast<ExtEOS*>(eos);
    if( ext == NULL )
    {
        ext = new Hydro(*eos);
        deleteEOS(eos);
    }
    return ext;
}

ExtEOS *FetchNewExtEOS(const char *type_name, DataBase &db)
{
    EOS *eos = static_cast<EOS*>(db.FetchNewObj("EOS",type_name));
    if( eos == NULL )
        return NULL;
    ExtEOS *ext = dynamic_cast<ExtEOS*>(eos);
    if( ext == NULL )
    {
        ext = new Hydro(*eos);
        deleteEOS(eos);
    }
    return ext;
}
