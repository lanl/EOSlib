#include <iostream>
#include <cstring>
#include "DataBaseIterators.h"
using namespace std;

static char *Strdupn(const char *src, int n)
{
    if( n == 0 )
        return NULL;
    char *dest = new char[n+1];
    int i;
    for( i=0; i<n; i++ )
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;   
}

static int SplitProperty(const char *property,
                               char *&base,char *&type,char *&name)
{
    if( property == NULL || property[0] == '\0' )
    {
        base = type = name = NULL;
        return -1;
    }
    int colon[3];
    int n = 0;
    int i;
    for(i=0; property[i] != '\0'; i++)
    {
        if( property[i] != ':' )
            continue;
        if( n == 3 )
        {
            base = type = name = NULL;
            return -1;
        }
        colon[n++] = i;
    }
    if( n == 0 )
    {
        base = type = NULL;
        name = Strdupn(property,i);
    }
    else if( n == 1 )
    {
        name = NULL;
        base = Strdupn(property, colon[0]);
        type = Strdupn(property+colon[0]+1, i-colon[0]-1);
    }
    else if( n == 2 )
    {
        if( colon[0]+1 != colon[1] )
        {
            base = type = name = NULL;
            return 1;
        }
        base = NULL;
        type = Strdupn(property, colon[0]);
        name = Strdupn(property+colon[1]+1, i-colon[1]-1);       
    }
    else
    {
        if( colon[1]+1 != colon[2] )
        {
            base = type = name = NULL;
            return 1;
        }
        base = Strdupn(property, colon[0]);
        type = Strdupn(property+colon[0]+1, colon[1]-colon[0]-1);
        name = Strdupn(property+colon[2]+1, i-colon[2]-1);       
    }
    if( base == NULL )
        base = strdup("");
    return 0;   
}

int DataBase::List(ostream &out, const char *property, int print)
{
    char *base;
    char *type;
    char *name;
    if( SplitProperty(property, base,type,name) )
        return 1;

    BaseIterator Bitr(*this);
    Base *b;
    for( Bitr.Start(base); b=Bitr; Bitr.Next() )
    {
        if( type == NULL )
        {
            out << b->base << "\n";
            continue;
        }
        TypeIterator Titr(*this,*b);
        Type *t;
        for( Titr.Start(type); t=Titr; Titr.Next() )
        {
            if( name == NULL )
            {
                out << b->base << ":" << t->type << "\n";
                continue;
            }
            ParameterIterator Pitr(*this,*t);
            Parameters *p;
            for( Pitr.Start(name); p=Pitr; Pitr.Next() )
            {
                if( print == 2 )
                {
                    if( !strcmp("Units",b->base) )
                    {
                        Units *units = FetchUnits(t->type, p->name);
                        if( units )
                            units->Print(out);
                    }
                    else if( Use(t->props, *p) )
                        out << b->base << ":" << t->type << "::" << p->name
                            << " Error applying use\n";
                    else
                        p->Print(out);                        
                }
                else if( print == 1 )
                    p->Print(out);
                else
                {
                    out << b->base << ":" << t->type << "::" << p->name;
                    if( print==0 && p->units && p->units[0] != '\0')
                        out << " Units:" << p->units;
                    out << "\n";
                }
            }
        }
    }
    return 0;
}
