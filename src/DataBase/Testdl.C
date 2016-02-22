#include <Arg.h>

#include <dlfcn.h>
#include <cstdlib>
#include <Arg.h>

int main(int, char **argv)
{
    typedef double (*FUNC)();           // function returning double
    ProgName(*argv);
    const char *shared = "./shared_obj.so";   // shared object
    const char *func   = "TestFunc";          // function name
        
    while(*++argv)
    {
        GetVar(shared,shared);
        GetVar(func,func);
            
        ArgError;
    }

/***
    void *main = dlopen(NULL,RTLD_NOW|RTLD_GLOBAL);
    if( main==NULL )
    {
        cerr << "dlopen failed: " << dlerror() << "\n";
        return 1;
    }
***/
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
