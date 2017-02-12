#include <string>
using namespace std;
#include "DataBase.h"

void *DataBase::FetchObj(Base &b, const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name, type,name) )
        return NULL;
        
   void *obj =  FetchObj(b, type, name);
   delete [] type;
   delete [] name;
   return obj;
}

void *DataBase::FetchNewObj(Base &b, const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name,type,name) )
        return NULL;
        
   void *obj =  FetchNewObj(b, type, name);
   delete [] type;
   delete [] name;
   return obj;
}

void *DataBase::FetchObj(const char *base, const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name, type,name) )
        return NULL;
        
   void *obj =  FetchObj(base, type, name);
   delete [] type;
   delete [] name;
   return obj;
}

void *DataBase::FetchNewObj(const char *base, const char *type_name)
{
    const char *type;
    const char *name;
    if( TypeName(type_name, type,name) )
        return NULL;
        
   void *obj =  FetchNewObj(base, type, name);
   delete [] type;
   delete [] name;
   return obj;
}

void *DataBase::FetchObj(Base &b, const char *type, const char *name)
{
    Property *p = FetchProp(b,type, name);
    if( p == NULL )
    {
        error->Log("DataBase::FetchObj", __FILE__, __LINE__, this,
                   "FetchProp failed for %s:%s::%s\n", b.base,type,name);
        return NULL;
    }
    if( p->obj == NULL && (p->obj=NewObject(*p)) == NULL )
    {
        error->Log("DataBase::FetchObj", __FILE__, __LINE__, this,
                   "NewObject failed for %s:%s::%s\n", b.base,type,name);
        return NULL;
    } 
    return b.Duplicate(p->obj);   
}

void *DataBase::FetchNewObj(Base &b, const char *type, const char *name)
{
    Property *p = FetchProp(b, type, name);
    if( p == NULL )
    {
        error->Log("DataBase::FetchNewObj", __FILE__, __LINE__, this,
                   "FetchProp failed for %s:%s::%s\n", b.base,type,name);
        return NULL;
    }
    return NewObject(*p);
}

void *DataBase::FetchObj(const char *base, const char *type, const char *name)
{
    Property *p = FetchProp(base, type, name);
    if( p == NULL )
    {
        error->Log("DataBase::FetchObj", __FILE__, __LINE__, this,
                   "FetchProp failed for %s:%s::%s\n", base,type,name);
        return NULL;
    }
    if( p->obj == NULL && (p->obj=NewObject(*p)) == NULL )
    {
        error->Log("DataBase::FetchObj", __FILE__, __LINE__, this,
                   "NewObject failed for %s:%s::%s\n", base,type,name);
        return NULL;
    } 
    return p->type->base->Duplicate(p->obj);   
}

void *DataBase::FetchNewObj(const char *base, const char *type, const char *n)
{
    Property *p = FetchProp(base, type, n);
    if( p == NULL )
    {
        error->Log("DataBase::FetchNewObj", __FILE__, __LINE__, this,
                   "FetchProp failed for %s:%s::%s\n", base,type,n);
        return NULL;
    }
    return NewObject(*p);
}

#define FUNC "DataBase::NewObject", __FILE__, __LINE__, this
void *DataBase::NewObject(Property &p)
{
    if( p.params == NULL )
        return NULL;
    Type &t = *p.type;
    Base &b = *t.base;
    if( t.Constructor == NULL )
    {
        Handle *handle;
        if( b.Initialize == NULL )
        {
            if( !(handle=GetHandle(b.base,b.base)) )
                return NULL;
            INIT init = (INIT) GetFunction(*handle, b.base,NULL, "Init");
            if( init )
            {
                char *msg = init();
                if( msg )
                {
                   error->Log(FUNC, "%s_Init() failed with message\n%s\n",
                              b.base, msg);
                   delete []msg;
                   return NULL;
                }
                INFO info = (INFO) GetFunction(*handle, b.base,NULL, "Info");
                if( info )
                    b.info = info();
            }
            b.Initialize = (INITIALIZE)
                            GetFunction(*handle, b.base,NULL,"Initialize");
            b.Duplicate  = (DUPLICATE)
                            GetFunction(*handle, b.base,NULL, "Duplicate");
            b.Destructor = (DESTRUCTOR)
                            GetFunction(*handle, b.base,NULL, "Destructor");
            if( b.Initialize==NULL || b.Duplicate==NULL || b.Destructor==NULL )
            {
                b.Duplicate   = NULL;
                b.Initialize  = NULL;
                b.Destructor  = NULL;
                return NULL;
            }
        }
        if( !(handle=GetHandle(b.base,t.type)) )
            return NULL;
        INIT init = (INIT) GetFunction(*handle, b.base,t.type, "Init");
        if( init )
        {
            char *msg = init();
            if( msg )
            {
               error->Log(FUNC, "%s_%s_Init() failed with message\n%s\n",
                          b.base, t.type, msg);
               delete []msg;
               return NULL;
            }
            INFO info = (INFO) GetFunction(*handle, b.base,t.type, "Info");
            if( info )
                t.info = info();
        }
        
        t.Constructor = (CONSTRUCTOR)
                         GetFunction(*handle, b.base,t.type,"Constructor");
        if( t.Constructor == NULL )
            return NULL;
    }
    if( Use(t.props, *p.params) )
    {
        error->Log(FUNC, "Use failed for `%s:%s::%s'\n",
                   b.base,t.type,p.params->name);
        return NULL;
    }
    void *obj = t.Constructor();
    if( obj == NULL )
    {
        error->Log(FUNC, "Constructor failed for `%s:%s'\n", b.base,t.type);
        return NULL;
    }
    if( b.Initialize(obj, *p.params, *this) )
    {
        error->Log(FUNC, "Initialization failed for `%s:%s::%s'\n",
                   b.base,t.type,p.params->name);
        b.Destructor(obj);
        return NULL;
    }
    return obj;
}

#undef FUNC
#define FUNC "DataBase::GetFunction", __FILE__, __LINE__, this
void *DataBase::GetFunction(Handle &handle, const char *base, const char *type,
                                            const char *name)
{
    string func(base);
    if( !strcmp(base,"Units") )
        type = "Units";
    if( type == NULL )
        func = func + "_" + name;
    else
        func = func + "_" + type + "_" + name;
	void *pfunc = handle.dlsym(func.c_str());
    if( pfunc == NULL )
        error->Log(FUNC, "dlsym failed to get `%s'\n"
                         "\tdlerror=%s", func.c_str(),handle.dlerror());
    return pfunc;
}

#undef FUNC
#define FUNC "DataBase::GetHandle", __FILE__, __LINE__, this
Handle *DataBase::GetHandle(const char *base, const char *type)
{
    string base_type(base);
    base_type = base_type + "::" + type;
    Handle *h = &handles[base_type.c_str()];
    if( *h )
        return h;
    Property *prop = FetchProp(*shared_libs, base);
    if( prop == NULL )
    {
        error->Log(FUNC, "no block SharedLibs=%s in database\n", base);
        return NULL;
    }
    const char *lib = prop->params->Value(type) ;
    if( lib == NULL )
    {
        error->Log(FUNC, "no shared library for %s::%s\n", base, type);
        return NULL;
    }
    if( !strcmp(lib,"NULL") )
    {
        if( main == NULL )
        {
            main = new Handle;
            main->dlopen(NULL);
            const char *dlmesg = main->dlerror();
            if( dlmesg )
            {
                error->Log(FUNC, "dlopen failed for NULL, %s\n", dlmesg);
                return NULL;
            }
        }
        h->copy(*main);
        return h;  
    }
    const char *shared_obj = FullPath(lib);
    if( shared_obj == NULL )
    {
        error->Log(FUNC, "FullPath failed for %s\n", lib);
        return NULL;
    }
    Handle &so = handles[shared_obj];
    if( !so && so.dlopen(shared_obj) )
    {
        error->Log(FUNC, "dlopen failed for `%s'\n%s\n", shared_obj,
                   so.dlerror() );
        delete [] shared_obj;
        return NULL;
    }
    delete [] shared_obj;
    h->copy(so);
    return h;
}

#undef FUNC
#define FUNC "DataBase::FullPath", __FILE__, __LINE__, this
const char *DataBase::FullPath(const char *lib)
{
    if( lib == NULL)
        return NULL;
    if( lib[0] == '/' )
        return strdup(lib);
    Parameters &params = *GetProp(*database,"Environment").params;
    if( lib[0] != '$' )
    {
        string str(params.Value("PWD"));
        str = str + "/" + lib;
        return strdup(str.c_str());
    }
    const char *libname = EnvironmentVar(lib+1);
    if( libname == NULL )
    {
        error->Log(FUNC, "lib environment variable, lib = %s\n",lib);
        return NULL;
    }
    return libname;
}
