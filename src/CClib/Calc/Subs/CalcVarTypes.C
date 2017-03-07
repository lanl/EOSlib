#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ? NULL : strdup(str);
}
#include "CalcVars.h"

CalcVar::~CalcVar() {}
int CalcVar::get(double &)      {return 1;}
int CalcVar::set(double  )      {return 1;}
int CalcVar::get(const char *&) {return 1;}
int CalcVar::set(const char *)  {return 1;}



CVdouble::CVdouble(double v) : CalcVar(DOUBLE), x(v) {}
CVdouble::~CVdouble()  {}
        
int CVdouble::get(double &value)
{
    value = x;
    return 0;
}
int CVdouble::set(double value)
{
    x = value;
    return 0;
}



CVpdouble::CVpdouble(double *v) : CalcVar(DOUBLE), x(v) {}
CVpdouble::~CVpdouble()  {}
        
int CVpdouble::get(double &value)
{
    value = *x;
    return 0;
}
int CVpdouble::set(double value)
{
    *x = value;
    return 0;
}


CVpinvdouble::CVpinvdouble(double *v) : CalcVar(DOUBLE), x(v) {}
CVpinvdouble::~CVpinvdouble()  {}
        
int CVpinvdouble::get(double &value)
{
    value = 1./(*x);
    return 0;
}
int CVpinvdouble::set(double value)
{
    *x = 1./value;
    return 0;
}


CVint::CVint(int v) : CalcVar(INT), x(v) {}
CVint::~CVint()  {}
        
int CVint::get(double &value)
{
    value = (double) x;
    return 0;
}
int CVint::set(double value)
{
    x = (int) value;
    return 0;
}



CVpint::CVpint(int *v) : CalcVar(INT), x(v) {}
CVpint::~CVpint()  {}
        
int CVpint::get(double &value)
{
    value = (double)(*x);
    return 0;
}
int CVpint::set(double value)
{
    *x = (int) value;
    return 0;
}



CVpstring::CVpstring(char **str) : CalcVar(STRING), string(str) {}
CVpstring::~CVpstring()  {}
        
int CVpstring::get(const char *&value)
{
    value = *string;
    return 0;
}
int CVpstring::set(const char *value)
{
    delete [] *string;
    *string = Strdup(value);
    return 0;
}



CVstring::CVstring(const char *str) : CalcVar(STRING)
{
    string = Strdup(str);
}

CVstring::~CVstring()
{
    delete [] string;
}
        
int CVstring::get(const char *&value)
{
    value = string;
    return 0;
}
int CVstring::set(const char *value)
{
    delete [] string;
    string = Strdup(value);
    return 0;
}

