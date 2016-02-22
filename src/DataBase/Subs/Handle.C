#include <dlfcn.h>
#include <cstdlib>
#include "Handle.h"

Handle::Handle() : handle(NULL), del(0)
{
    // Null
}

Handle::~Handle()
{
    if( del )
        dlclose(handle);
}

void Handle::copy(const Handle &h)
{
    handle = h.handle;
    del = 0;
}

int Handle::dlopen(const char *shared_obj)
{
    handle = ::dlopen(shared_obj, RTLD_NOW | RTLD_GLOBAL);
    if( handle )
    {
        del = 1;
        return 0;
    }
    else
        return 1;
}

const char *Handle::dlerror()
{
    return ::dlerror();
}

void *Handle::dlsym(const char *func)
{
    return ::dlsym(handle,func);
}
