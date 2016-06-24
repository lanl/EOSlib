#include <dlfcn.h>
#include <cstdlib>
#include "Handle.h"
#include <sstream>

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

#ifdef __APPLE__
#define SHARED_LIB_EXT ".dylib"
#elif __unix__
#define SHARED_LIB_EXT ".so"
#elif _WIN32
#define SHARED_LIB_EXT ".dll"
#endif

int Handle::dlopen(const char *shared_obj)
{
    std::ostringstream oss;
    oss << shared_obj << SHARED_LIB_EXT;
    //    handle = ::dlopen(shared_obj, RTLD_NOW | RTLD_GLOBAL);
    auto s = oss.str();
    handle = ::dlopen(s.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if( handle )
    {
        del = 1;
        return 0;
    }
    else
        return 1;
}

#ifdef SHARED_LIB_EXT
#undef SHARED_LIB_EXT
#endif

const char *Handle::dlerror()
{
    return ::dlerror();
}

void *Handle::dlsym(const char *func)
{
    return ::dlsym(handle,func);
}
