#include "DataBaseIterators.h"

BaseIterator::BaseIterator(DataBase &DB)
                : db(DB), base(NULL), names(NULL), N(0),i(0)
{
    // Null
}
    
int BaseIterator::Start(const char *b)
{
    int status = 0;
    i = N = 0;
    base = NULL; 
    char *hname = NULL;
    if( b == NULL )
    {
        hname   = db.HashName("");
        base = &db.bases[hname];
    }
    else if( !strcmp(b,"*") )
    {
        delete [] names;
        names = db.bases.Names();
        N = db.bases.Dimension();
    }
    else
    {
        hname   = db.HashName(b);
        if( db.bases.Defined(hname) )
            base = &db.bases.LastElement();
        else
            status = 1;
    }
    delete [] hname;
    return status;
}

int BaseIterator::Next()
{
    if( base )
    {
        base = NULL;
        return 0;
    }
    else if( ++i < N )
        return 1;
    else
        return 0;  
}

BaseIterator::operator Base*()
{
    if( base )
        return base;
    else if( i < N )
        return db.bases.Defined(names[i]) ? &(db.bases.LastElement()) : NULL;
    else
        return NULL;
}

//

TypeIterator::TypeIterator(DataBase &DB, Base &B)
                : db(DB),base(B), type(NULL), names(NULL),N(0),i(0)
{
    // Null
}
    
int TypeIterator::Start(const char *t)
{
    int status = 0;
    i = N = 0;
    type = NULL; 
    char *hname = NULL;
    if( t == NULL )
    {
        status = 1;
    }
    else if( !strcmp(t,"*") )
    {
        delete [] names;
        names = base.types.Names();
        N = base.types.Dimension();
    }
    else
    {
        hname = db.HashName(t);
        if( base.types.Defined(hname) )
            type = &base.types.LastElement();
        else
            status = 1;
    }
    delete [] hname;
    return status;
}

int TypeIterator::Next()
{
    if( type )
    {
        type = NULL;
        return 0;
    }
    else if( ++i < N )
        return 1;
    else
        return 0;  
}

TypeIterator::operator Type*()
{
    if( type )
        return type;
    else if( i < N )
        return base.types.Defined(names[i]) ?
                                    &(base.types.LastElement()) : NULL;
    else
        return NULL;
}

//

ParameterIterator::ParameterIterator(DataBase &DB, Type &T)
                : db(DB), type(T),param(NULL), names(NULL),N(0),i(0)
{
    // Null
}

int ParameterIterator::Start(const char *n)
{
    int status = 0;
    i = N = 0;
    param = NULL; 
    char *hname = NULL;
    if( n == NULL )
    {
        status = 1;
    }
    else if( !strcmp(n,"*") )
    {
        delete [] names;
        names = type.props.Names();
        N = type.props.Dimension();
    }
    else
    {
        hname = db.HashName(n);
        if( type.props.Defined(hname) )
            param = type.props.LastElement().params;
        else
            status = 1;
    }
    delete [] hname;
    return status;
}

int ParameterIterator::Next()
{
    if( param )
    {
        param = NULL;
        return 0;
    }
    else if( ++i < N )
        return 1;
    else
        return 0;  
}

ParameterIterator::operator Parameters*()
{
    if( param )
        return param;
    else if( i < N )
        return type.props.Defined(names[i]) ?
                                    type.props.LastElement().params : NULL;
    else
        return NULL;
}


