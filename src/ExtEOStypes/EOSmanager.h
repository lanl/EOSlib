#ifndef EOSLIB_EOS_MANAGER_H
#define EOSLIB_EOS_MANAGER_H

#include "EOS.h"
#include "String.h"
#include "EOSstate.h"
#include "PointerArray.h"

#include <iostream>


// ename = type::name       (database_name = EOS:type::name)
// uname = name             (database_name = Units:hydro::name)

class EOS_Array : public PointerArray<EOS>
{
private:
    void operator=(const EOS_Array&);           // disallowed
    EOS_Array(const EOS_Array&);                // disallowed
public:
    EOS_Array(int Dim = 32) : PointerArray<EOS>(Dim) {}
    ~EOS_Array();
    int Find(const char *ename, const char *uname);
};


class Units_Array : public PointerArray<Units>
{
private:
    void operator=(const Units_Array&);           // disallowed
    Units_Array(const Units_Array&);              // disallowed
public:
    Units_Array(int Dim = 8) : PointerArray<Units>(Dim) {}
    ~Units_Array();
};

class EOSmanager : protected DataBase
{
private:
    EOSmanager(const EOSmanager&);               // disallowed
    void operator=(const EOSmanager&);           // disallowed
protected:
    char *env;
    char *data_files;

    EOS_Array e;
    Units_Array u;
    PointerArray<EOSstate> s;
    EOSlib::STATUS status;
public:
    ostream &EOSlog;
    EOSmanager(ostream &log = std::cerr);
    virtual ~EOSmanager();
    int Read(const char *files);
    void SetEnv(const char *Env);

         EOS &eos  (int i) { return *e[i]; }
       Units &units(int i) { return *u[i]; }
    EOSstate &state(int i) { return *s[i]; }

    int valid_eos  (int i) { return 0<=i && i<e.N(); }
    int valid_units(int i) { return 0<=i && i<u.N(); }
    int valid_state(int i) { return 0<=i && i<s.N(); }

    int index_eos  (const char *name, const char *unit);
    int index_units(const char *name);
    int index_state(const char *name)               { return s.Find(name);    }
    int AddState(const char *name, EOSstate *&ptr)  { return s.Add(name,ptr); }
    int ReplaceState(const char *name, EOSstate *&ptr);
    int List(ostream &out, const char *property, int flag = -1);
        
    EOSlib::STATUS Status() const {return status;}
};

inline int EOSmanager::ReplaceState(const char *name, EOSstate *&ptr)
    { return s.Replace(name,ptr); }
inline int EOSmanager::List(ostream &out, const char *property, int flag)
    { return DataBase::List(out,property, flag); }


#endif // EOSLIB_EOS_MANAGER_H
