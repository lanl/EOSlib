#include "CalcVars.h"

CalcVar::TYPE CalcVars::type(const char *name)
{
    if( variables.Defined(name) )
        return variables.LastElement().Type();
    else
        return CalcVar::UNDEF;
}

CalcVar *CalcVars::Fetch(const char *name)
{
    return variables.Defined(name) ? &variables.LastElement() : NULL;
}

int CalcVars::AddVar(const char *name, CalcVar *&var)
{
    if( variables.Defined(name) )
        return 1;
    variables.Replace(name,var);
    var = NULL;
    return 0;      
}

int CalcVars::AddVar(const char *name, int *x)
{
    if( variables.Defined(name) )
        return 1;

    CVpint *var = new CVpint(x); 
    variables.Replace(name,var);
    return 0;      
}
int CalcVars::AddVar(const char *name, int x)
{
    if( variables.Defined(name) )
        return 1;

    CVint *var = new CVint(x); 
    variables.Replace(name,var);
    return 0;      
}

int CalcVars::AddVar(const char *name, double *x)
{
    if( variables.Defined(name) )
        return 1;

    CVpdouble *var = new CVpdouble(x); 
    variables.Replace(name,var);
    return 0;      
}
int CalcVars::AddVar(const char *name, double x)
{
    if( variables.Defined(name) )
        return 1;

    CVdouble *var = new CVdouble(x); 
    variables.Replace(name,var);
    return 0;      
}

int CalcVars::AddVar(const char *name, char **x)
{
    if( variables.Defined(name) )
        return 1;

    CVpstring *var = new CVpstring(x); 
    variables.Replace(name,var);
    return 0;      
}
int CalcVars::AddVar(const char *name, const char *x)
{
    if( variables.Defined(name) )
        return 1;

    CVstring *var = new CVstring(x); 
    variables.Replace(name,var);
    return 0;      
}


int CalcVars::get(const char *name, double &value, double *undef)
{
    if( variables.Defined(name) )
        return variables.LastElement().get(value);
    else if( undef )
    {
        value = *undef;
        CVdouble *var = new CVdouble(value); 
        variables.Replace(name,var);
        return 0;
    }
    else
        return 1;
}

int CalcVars::set(const char *name, double value, int undef)
{
    if( variables.Defined(name) )
        return variables.LastElement().set(value);
    else if( undef )
    {
        CVdouble *var = new CVdouble(value); 
        variables.Replace(name,var);
        return 0;
    }
    else
        return 1;
}

int CalcVars::get(const char *name, const char *&value, const char *undef)
{
    if( variables.Defined(name) )
        return variables.LastElement().get(value);
    else if( undef )
    {
        CVstring *var = new CVstring(undef);
        variables.Replace(name,var);
        var->get(value);
        return 0;
    }
    else
        return 1;
}

int CalcVars::set(const char *name, const char *value, int undef)
{
    if( variables.Defined(name) )
        return variables.LastElement().set(value);
    else if( undef )
    {
        CVstring *var = new CVstring(value); 
        variables.Replace(name,var);
        return 0;
    }
    else
        return 1;
}
