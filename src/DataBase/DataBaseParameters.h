#ifndef EOSLIB_DATA_BASE_Parameters_H
#define EOSLIB_DATA_BASE_Parameters_H

#include <iosfwd>

class Parameters;
class ParamIterator;
class Calc;

int UnitsMismatch(const Parameters &p, const Parameters &q);
// units = "utype::uname",  ex: hydro::MKS
//       = "",              no units, matches only ""
//       = "*",             unspecified, no mismatch
//       = NULL,            undefined, always mismatch

class Parameters
{
    friend class ParamIterator;
    friend class DataBase;
private:
    int dim;
    void Init();
    void ReAlloc();

    int Nparams;
    int use;
    char **param;
    char **value;
    
    Parameters(Parameters &p, int d=0);         // Transfer
    Parameters(const Parameters&);              // disallowed
    void operator=(const Parameters&);          // disallowed
public:
    char *name;
    char *type;
    char *base;
    char *units;
    
    Parameters(int Dim = 16);
    ~Parameters();
    
    int Add(const char *Param, const char *Value);
    int Add(const char *Param, double Value);
    int Add(const char *line);
    int Append(const char *Param, const char *Value);
    int Append(const char *Param, double Value);
    int Append(const char *line);
    int Append(const Parameters &Uparam);
    int Prepend(const Parameters &Uparam);
    void DeleteLast();
    const char *Value(const char *Param) const;
    
    std::ostream &Print(std::ostream &out) const;
    Parameters *Copy();
    
    int Use() const {return use;}
    void Expand();
};

class ParamIterator
{
private:
    const Parameters *params;
    int i;
    char *line;
        
    ParamIterator(const ParamIterator&);        // disallowed
    void operator=(const ParamIterator&);       // disallowed
public:
    ParamIterator(const Parameters &p) : params(&p), i(0), line(NULL) {}
    ~ParamIterator() { delete[]line;}
    
    operator int() { return i <  params->Nparams; }
    int Next()     { return (++i < params->Nparams) ? 1 : 0;}
    int Next(const char *&param, const char *&value);
    const char *Param() {return (i<params->Nparams) ? params->param[i] : NULL;}
    const char *Value() {return (i<params->Nparams) ? params->value[i] : NULL;}
    char *Line();
    void Reset() { i=0;}
    char *Parse(Calc &calc);
};


#endif     // EOSLIB_DATA_BASE_Parameters_H
