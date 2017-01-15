#ifndef EOSLIB_EXT_EOS_MANAGER_H
#define EOSLIB_EXT_EOS_MANAGER_H

#include <iostream>
#include "../Manager/EOSmanager.h"
#include "ExtEOSstate.h"

class ExtEOS_Array : public PointerArray<ExtEOS>
{
private:
    void operator=(const ExtEOS_Array&);           // disallowed
    ExtEOS_Array(const ExtEOS_Array&);             // disallowed
public:
    ExtEOS_Array(int Dim = 32) : PointerArray<ExtEOS>(Dim) {}
    ~ExtEOS_Array();
};


class ExtEOSmanager : protected EOSmanager
{
private:
    ExtEOSmanager(const ExtEOSmanager&);            // disallowed
    void operator=(const ExtEOSmanager&);           // disallowed
protected:
    ExtEOS_Array ex;
    PointerArray<ExtEOSstate> sx;
public:
    ExtEOSmanager(std::ostream &log = std::cerr) : EOSmanager(log) {}
    ~ExtEOSmanager();
    using EOSmanager::EOSlog;
    using EOSmanager::Read;
    using EOSmanager::SetEnv;

    ExtEOS &xeos  (int i)    { return *ex[i];            }
    using EOSmanager::eos;
    using EOSmanager::units;
    ExtEOSstate &state(int i)   { return *sx[i];            }

    using EOSmanager::valid_eos;
    using EOSmanager::valid_units;
    int valid_state(int i)      { return 0<=i && i<sx.N();  }

    int index_eos(const char *name, const char *unit);
    using EOSmanager::index_units;
    int index_state(const char *name);
    int AddState(const char *name, ExtEOSstate *&ptr);
    int ReplaceState(const char *name, ExtEOSstate *&ptr);
    using EOSmanager::List;
    using EOSmanager::Status;
};

inline int ExtEOSmanager::index_state(const char *name)
    { return sx.Find(name);         }
inline int ExtEOSmanager::AddState(const char *name, ExtEOSstate *&ptr)
    { return sx.Add(name,ptr);      }
inline int ExtEOSmanager::ReplaceState(const char *name, ExtEOSstate *&ptr)
    { return sx.Replace(name,ptr);  }


#endif // EOSLIB_EXT_EOS_MANAGER_H
