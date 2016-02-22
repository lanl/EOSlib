#ifndef EOSLIB_HANDLE_H
#define EOSLIB_HANDLE_H


class Handle
{
private:
    void *handle;
    int del;
    Handle(const Handle &h);            // disallowed
    void operator=(const Handle &h);    // disallowed
public:
    Handle();
    ~Handle();
    void copy(const Handle &h);
    int dlopen(const char *shared_obj);
    const char *dlerror();
    void *dlsym(const char *func);
    operator int () {return handle != NULL;}
};



#endif // EOSLIB_HANDLE_H

