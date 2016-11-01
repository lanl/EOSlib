#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <String1.h> // SplitString

#include "DataBase.h"

using namespace std;

#define MAX_PARAMS 16

int UnitsMismatch(const Parameters &p, const Parameters &q)
{
    if( p.units == NULL || q.units == NULL )
        return 1;
    if( !strcmp(p.units,"*") || !strcmp(q.units,"*") )
        return 0;
    return strcmp(p.units,q.units);
}


void Parameters::Init()
{
    if( dim <=0 )
        dim = MAX_PARAMS;
        
    param = new char*[dim];
    value = new char*[dim];
    int i;
    for(i=0; i<dim; i++)
    {
        param[i] = NULL;
        value[i] = NULL;
    }
    Nparams = 0;
    use = 0;
}

void Parameters::ReAlloc()
{
    int dim0 = dim;
    char **param0 = param;
    char **value0 = value;
    
    dim *= 2;
    param = new char*[dim];
    value = new char*[dim];
    
    int i;
    for( i=0; i < dim0; i++)
    {
        param[i] = param0[i];
        value[i] = value0[i];
    }
    do
    {
        param[i] = 0;
        value[i] = 0;
    } while ( ++i < dim );
    
    delete [] param0;
    delete [] value0;
}


Parameters::Parameters(int dim0)
           : dim(dim0), name(NULL),type(NULL),base(NULL), units(NULL)
{
    Init();
}


Parameters *Parameters::Copy()
{
    Parameters *p = new Parameters(Nparams);
    p->name  = strdup(name);
    p->type  = strdup(type);
    p->base  = strdup(base);
    p->units = strdup(units);
    p->use   = use;
    
    int i;
    for(i=0; i<Nparams; i++)
        p->Append(param[i], value[i]);
    
    return p;
}

Parameters::Parameters(Parameters &p, int d)    // Transfer
{
    dim = p.dim;
    Nparams = p.Nparams;

    param = p.param;
    value = p.value;
    use   = p.use;
    
    name = strdup(p.name);
    type = strdup(p.type);
    base = strdup(p.base);
    units = strdup(p.units);
    
    p.dim = d > 0 ? d : 2*dim;
    p.Init();    
}


Parameters::~Parameters()
{
    delete [] name;
    delete [] type;
    delete [] base;
    delete [] units;

    int i;
    for(i=0; i<Nparams; i++)
    {
        delete [] param[i];
        delete [] value[i];
    }
    
    delete [] param;    
    delete [] value;    
}

int Parameters::Add(const char *Param, const char *Value)
{
    if( Param == NULL || Value == NULL )
        return 1;
    int i;
    for( i=0; i<Nparams; i++ )
    {
        if( !strcmp(Param, param[i]) )
        {
            delete [] value[i];
            value[i] = strdup(Value);
            return 0;
        }
    }
    return Append(Param, Value);
}
int Parameters::Add(const char *Param, double Value)
{
    //string svalue = to_string(Value);     # C++11
    ostringstream ss;
    ss << Value;
    int status = Add(Param, ss.str().c_str());
    return status;
}
int Parameters::Add(const char *line)
{
    SplitString Line(line);
    const char *param = Line.WordTo("=");
    const char *value = Line.Remainder();
    return Add(param,value);
}

int Parameters::Append(const char *Param, const char *Value)
{
    if( Param == NULL || Value == NULL )
        return 1;
    if( Nparams == dim )
        ReAlloc();
        
    param[Nparams] = strdup(Param);
    value[Nparams] = strdup(Value);
    Nparams++;
    if( !strcmp(Param, USE) )
        use++;
    
    return 0;
}
int Parameters::Append(const char *Param, double Value)
{
    //string svalue = to_string(Value);     # C++11
    ostringstream ss;
    ss << Value;
    int status = Append(Param, ss.str().c_str());
    return status;
}
int Parameters::Append(const char *line)
{
    SplitString Line(line);
    const char *param = Line.WordTo("=");
    const char *value = Line.Remainder();
    return Append(param,value);
}

void Parameters::DeleteLast()
{
    if( Nparams == 0 )
        return;
        
    Nparams--;
        
    if( param[Nparams]!= NULL && !strcmp(param[Nparams], USE) )
        use--;
        
    delete [] param[Nparams];
    delete [] value[Nparams];
    param[Nparams] = NULL;
    value[Nparams] = NULL;
}

int Parameters::Prepend(const Parameters &Uparams)
{
    dim = Nparams + Uparams.Nparams;
    Parameters old(*this,dim);
    return Append(Uparams) || Append(old);
}

int Parameters::Append(const Parameters &Uparams)
{
    const char *p;
    const char *v;    
    ParamIterator U(Uparams);
    while( U.Next(p,v) )
    {
        if( Append(p, v) )
            return 1;
    }
    return 0;
}

void Parameters::Expand()
{
    int i;
    for( i=0; i<Nparams; i++ )
    {
        char *v = value[i];
        if( v[0] == '$' )
        {
            const char *v1 = Value(v+1);
            if( v1 != NULL )
            {
                value[i] = strdup(v1);
                delete v;
            }
        }
    }
}

ostream &Parameters::Print(ostream &out) const
{
    if( base )
        out << base << ":";
    out << type << "=" << name;
    if( units[0] != '\0' )
        out << "; \tunits = " << units;
    if( use )
        out << " \t#USE = " << use;
    out << "\n{\n";

    FmtFlags old = out.setf(ios::left, ios::adjustfield);
    
    int i;
    for(i=0;i<Nparams; i++)
        out << "\t" << setw(20) << param[i]
            << " = " << value[i] << "\n";
    out << "}\n";

    out.setf(old, ios::adjustfield);
    return out;
}


const char *Parameters::Value(const char *Param) const
{
    if( Param == NULL )
        return NULL;
        
    int i;
    for(i=0; i<Nparams; i++)
    {
        if( !strcmp(Param, param[i]) )
            return value[i];
    }
    return NULL;
}
