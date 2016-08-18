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
    if( shared_obj == 0) return 1;  // empty string
    std::string str(shared_obj);
    // check and remove extension
    std::size_t pos = str.find(".");
    if( pos < std::string::npos)
      str.erase(pos, std::string::npos);
    // add new extension
    str += SHARED_LIB_EXT;

    handle = ::dlopen(str.c_str(), RTLD_NOW | RTLD_GLOBAL);
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
