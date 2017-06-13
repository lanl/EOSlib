#ifndef EOSLIB_UNITS_H
#define EOSLIB_UNITS_H

#include <iosfwd>
#include <Calc.h>
using std::ostream;
//
#define UNITS_vers "EOSv2.0.1"
#define UNITS_date "Mar. 13, 2016"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
int IsFundamental(const char *name);

// units = "utype::uname",  ex: hydro::MKS

class Units
{
    friend class Convert;
    friend int deleteUnits(Units *&);
private:
    int ref_count;

    char *utype;
    char *uname;
    
    Parameters *values;
    Parameters *names;

    Units(const Units&);                // disallowed
    void operator=(const Units&);       // disallowed
    Units *operator &();                // disallowed, use Duplicate()
public:
    static const char *fundamental[];
    Units() : utype(NULL),uname(NULL), values(NULL),names(NULL), ref_count(1) {}
    Units(const char *type, const char *name,
                            Parameters *&Values, Parameters *&Names);
    Units *Duplicate();
    int Initialize(Parameters &u, DataBase &list);
    int Initialize(const char* type, const char* name, DataBase &list);
    ~Units();
    
    int AddValue(const char *name, const char *value);
    int AddName( const char *name, const char *value);
    int Match(const Units &u) const { return Match(u.utype,u.uname); }
    int Match(const char *type, const char *name) const;
    int Match(const char *type_name) const;

    const char *Type() const { return utype; }
    const char *Name() const { return uname; }
    const char *Unit(const char *name) const;
    const char *Value(const char *name) const;

    ostream &Print(ostream &out, int rel_std=1) const;
    ostream &PrintNames(ostream &out) const { return Print(out,0); }
    ostream &PrintVarNames(ostream &out) const;
};

inline int Units::Match(const char *type, const char *name) const
{
    return !strcmp(utype,type) && !strcmp(uname,name);
}

inline int operator==(const Units &u1, const Units &u2)
{
    return !strcmp(u1.Type(), u2.Type()) && !strcmp(u1.Name(), u2.Name());
}

inline int operator!=(const Units &u1, const Units &u2)
{
    return strcmp(u1.Type(), u2.Type()) || strcmp(u1.Name(), u2.Name());
}

inline int Units::AddValue(const char* name, const char* value)
            { return values ? values->Add(name, value) : 1; }
inline int Units::AddName(const char* name, const char* value)
            { return names ? names->Add(name, value) : 1; }

int deleteUnits(Units *&);
extern "C" {
  int  Units_Initialize(void *, Parameters &, DataBase &);
  void *Units_Duplicate(void *);
  void Units_Destructor(void *);
  void *Units_hydro_Constructor();
}

class Convert
{
private:
    int status;

    Units *from;
    Units *to;
    
    Calc calc;
    Line line;
    void Statement(const char *param, const char *value);
    void LoadFundamentalUnits(Calc &calc, Parameters &values);

    Convert(const Convert&);            // disallowed
    void operator=(const Convert&);     // disallowed
public:
    Convert(Units &from, Units &to);
    ~Convert();
    operator int() {return status;}
        
    double factor(const char *quantity);
    Units *From() { return from ? from->Duplicate() : NULL; }
    Units *To()   { return   to ?   to->Duplicate() : NULL; }
    int MatchTo(Units *u);
    int MatchFrom(Units *u);
};

inline int Convert::MatchTo(Units *u)
{
    if( u == NULL || to == NULL )
        return 0;
    return to->Match(*u);
}
inline int Convert::MatchFrom(Units *u)
{
    if( u == NULL || from == NULL )
        return 0;
    return from->Match(*u);
}

#endif // EOSLIB_UNITS_H
