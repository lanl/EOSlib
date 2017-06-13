#include <Arg.h>

#include <dlfcn.h>
#include <cstdlib>

using namespace std;

int main(int, char **argv)
{
    typedef double (*FUNC)();           // function returning double
    ProgName(*argv);

// shared object
#ifdef __APPLE__
    const char *shared = "./libshared_obj.dylib";
#elif __unix__
    const char *shared = "./libshared_obj.so";
#elif _WIN32
    const char *shared = "./libshared_obj.dll";
#endif

    const char *func   = "TestFunc";          // function name
        
    while(*++argv)
    {
        GetVar(shared,shared);
        GetVar(func,func);
            
        ArgError;
    }

// test dynamic link from global handle
    void *main = dlopen(NULL,RTLD_NOW|RTLD_GLOBAL);
    if( main==NULL )
    {
        cerr << "dlopen failed: " << dlerror() << "\n";
        return 1;
    }
    else
    {
        void *pfunc = dlsym(main,"TestFunc1");
        if( pfunc==NULL )
        {
            cerr << "dlsym failed: " << dlerror() << "\n";
            return 1;
        }
        FUNC F = (FUNC)pfunc;
        cerr << "func " << F() << "\n";
    }
    dlclose(main);

// test dynamic link from shared library
    void *handle = dlopen(shared,RTLD_NOW|RTLD_GLOBAL);
    if( handle==NULL )
    {
        cerr << "dlopen failed: " << dlerror() << "\n";
        return 1;
    }
    void *pfunc = dlsym(handle,func);
    if( pfunc==NULL )
    {
        cerr << "dlsym failed: " << dlerror() << "\n";
        return 1;
    }
    FUNC F = (FUNC)pfunc;
    cerr << "func " << F() << "\n";
    
    dlclose(handle);
    return 0;
}
