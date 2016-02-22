#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
#include "DataBase.h"

static int split(char e, char *s)
{
    if( s == NULL )
        return -1;
    int i;
    for( i=0; s[i] != '\0'; i++ )
    {
        if( s[i] == e )
        {
            s[i] = '\0';
            return i+1;
        }
    }
    return -1;
}

int DataBase::BlockId(Parameters &p)
{
    char *s = vars->data->LastWord();
    if( s[0] == ' ' || s[0] == '\t' )
        return ERR;
    
    s= DeleteSpace(s,4);
    int n = split('=',s);
    if( n < 2 || s[n] == '\0' )
        return ERR;
    p.name = Strdup(s+n);
    
    n -= 2;
    int eq;
    switch (s[n]) {
      case '.':
          s[n] = '\0';
          eq = ADD;
          break;
      case '!':
          s[n] = '\0';
          eq = REPLACE;
          break;
      case '?':
          s[n] = '\0';
          eq = EXIST;
          break;
      default :
          eq = NEW;
          break;
    }
    if( s[0] == '\0' )
        return ERR;
    n = split(':',s);
    if( n<0 )
    {
        p.base = Strdup(vars->base);
        p.type = Strdup(s);
    }
    else
    {
        if( s[n] == '\0' )
            return ERR;
        p.base = Strdup(s);
        p.type = Strdup(s+n);
    }
    return eq;
}

/*
[base:]type [.,!,?]= name [; units=type::name]
{
    parameter = value [; parameter = value]
    . . .
}
*/ 
#define FUNC "DataBase::ReadParameters", __FILE__, __LINE__, this
int DataBase::ReadParameters(Parameters &p)
{
    int eq = BlockId(p);
    if( eq <= 0 )
    {
        error->Log(FUNC, "BlockId failed");
        return ERR;
    }
    DataFile &data = *vars->data;
    char *s;
    if( (s=data.NextWord(1)) )
    {
        while( s[0] == ' ' || s[0] == '\t' )
            ++s;
        if( data.words > 0 || strncmp(s,"units",5) )
        {
            error->Log(FUNC, "expect 'units'");
            return ERR;
        }
        s = DeleteSpace(s+5);
        if( *s == '=' )
            ++s;
    }
    else
        s = vars->units;
    p.units=Strdup(s);   

    s = data.NextWord(0);    
    if( s==NULL || s[0]!='{' || data.NextWord(1,2)  )
    {
        error->Log(FUNC, "expect '{'");
        return ERR;
    }
    while( data.NextLine() )
    {
        if( *data.Peek() == '}' )
        {
            s = data.NextWord();
            if( data.NextWord(1,2) ) 
            {
                error->Log(FUNC, "expect '}'");
                return ERR;
            }
            else
                return eq;
        }
        int n = 0;
        while( (s=data.NextWord(1,2)) )
        {
            n = split('=',s);
            if( n < 0 )
            {
                error->Log(FUNC, "expect 'param = value'");
                return ERR;
            }
            p.Append(s,s+n);
        }
    }
    error->Log(FUNC, "expect '}'");
    return ERR;
}
