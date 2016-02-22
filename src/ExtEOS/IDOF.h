#ifndef EOSLIB_IDOF_H
#define EOSLIB_IDOF_H

#include <iosfwd>

class Calc;
class Convert;

class IDOF      // base class for Internal Degree Of Freedom
{
private:
    void operator=(const IDOF &);       // disallowed
protected:
    int n;                              // number of variables
    double *z;                          // array  of variables
    // set names to static variables in derived class
    IDOF(const char *dname, const char **var_dname, int nn);
    IDOF(const IDOF &);
public:
    const char *name;                   // IDOF name
    const char **var_name;              // name of z[i]
    int N() const                   { return n;     }
    double  operator()(int i) const { return z[i];  }
    double &operator[](int i)       { return z[i];  }
    double *operator&()             { return z;     }
    int var_index(const char *vname) const;
 //
    virtual ~IDOF();
    int Replace(const IDOF &d);     // z[i] = d.z[i]
    virtual IDOF *Copy();           // NULL unless provided by derived class
 //
    virtual int InitParams(Calc &);
    virtual void Load(Calc &, double *);
    virtual void SetParams(Calc &);
 //
    virtual int ConvertParams(Convert &);
 //
    virtual void PrintParams(std::ostream &);
    virtual std::ostream &Print(std::ostream &, const double *);
};

#endif // EOSLIB_IDOF_H
