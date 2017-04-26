#ifndef EOSLIB_EOS_BASE_H
#define EOSLIB_EOS_BASE_H

//

#include <DataBase.h>

class EOSbase
{
    friend int deleteEOSbase(EOSbase *, const char*);
public:
    class Error;
    friend class Error;
    static double NaN;
    static int max_ref_count;
    static Error *ErrorDefault;                 // default error handler
    static int  initbase;
    static void Initbase();
protected:
    char *base;                                 // base is EOS interface
    char *type;                                 // class type :: base
    char *name;                                 // name for parameters
    Units *units;
    int ref_count;
    
    EOSlib::STATUS EOSstatus;
    Error *EOSerror;                            // error handler
    int Copy(const EOSbase &b, int check = 1);
    
    EOSbase(const EOSbase&);
    EOSbase(const char *ebase, const char *etype = NULL); 
    virtual int InitBase(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void  PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);
private:
    EOSbase();                                  // disallowed
    EOSbase *operator &();                      // disallowed, use Duplicate()
    void operator=(const EOSbase&);             // disallowed
public:
    int verbose;                                // flag for diagnostic output
    int RefCount() const { return ref_count; }
    int Initialize(const char *pname, DataBase &db, const char *uname=NULL);
    int Initialize(Parameters &, DataBase *db=NULL, Units *to=NULL);   
    virtual ~EOSbase();
 // material property = base:type::name
    const char* Base() const        { return base; }
    const char* Type() const        { return type; }
    const char* Name() const        { return name; }
    void Rename(const char *mname);
 // status
    EOSlib::STATUS status() const   { return EOSstatus; }
    EOSlib::STATUS status(EOSlib::STATUS s);
    const char *Status() const;
 // misc
    Error *ErrorHandler(Error *handler=NULL);
    EOSbase *Duplicate();   
 // print
    ostream &Print(ostream &out);
    virtual ostream &PrintComponents(ostream &out);
    ostream &PrintAll(ostream &out);
    ostream &PrintUnits(ostream &out) const;
 // units
    const char *Unit(const char *u_name);
    Units *DupUnits()       { return units ? units->Duplicate() : NULL; }
    const Units *UseUnits() { return units; }
    int ConvertUnits(Convert &convert);
    int ConvertUnits(Units &To);
    int ConvertUnits(const char *type_name, DataBase &db);
    int ConvertUnits(const char *u_type, const char *u_name, DataBase &db);
        // return status: 0 = successful
        //                1 = error, unchanged
        //               -1 = error, ALL FOULED UP
};
//
int deleteEOSbase(EOSbase *, const char*);

class EOSbase::Error                     // EOSbase error handler
{
public:
    EOSbase *eos;
    ostream *out;
    Error();
    Error(ostream &Out);
    virtual ~Error();
    virtual double Log(const char *func,  const char *file, int line,
                       EOSbase *Eos, const char *format, ...);
    virtual void Abort(const char *func, const char *file, int line,
                       EOSbase *Eos, const char *format, ...);
    virtual void Throw(const char *func, const char *file, int line,
                       EOSbase *Eos, const char *format, ...);
};
extern EOSbase::Error *EOScerr;
//
// inline EOSbase functions
//
inline const char *EOSbase::Status() const
{
    return EOSlib::Status(EOSstatus);
}
inline EOSlib::STATUS EOSbase::status(EOSlib::STATUS s)
{
    EOSlib::STATUS old=EOSstatus;
    EOSstatus=s;
    return old;
}

inline ostream &EOSbase::PrintAll(ostream &out)
{
    return PrintComponents(Print(out));
}
inline ostream &EOSbase::PrintUnits(ostream &out) const
{
    if( units )
        units->PrintNames(out);
    return out;
}

inline const char *EOSbase::Unit(const char *u_name)
{
    return units == NULL ? NULL : units->Unit(u_name);
}

#endif // _EOSbase_BASE_H
