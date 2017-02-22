#include "DataBase.h"
#include <cstring>

inline char *Strdup(const char *str)
{
    return (str==NULL) ?  NULL : strdup(str);
}

static char *dirname(const char *file)
{
    int len = strlen(file)-1;
    for( ; len>=0; len-- )
    {
        if( file[len] == '/' )
            break;
    }
    len++;
    char *dir = new char[len+1];
    dir[len]='\0';
    return (char *)memcpy(dir,file,len);
}

//

DBvars::DBvars(const char *f, DBvars *p) : prev(p) 
{
    ifstream *in = new ifstream;
	in->open(f);
	if( !(*in) )
    {
        delete in;
        file = dir = base = units = NULL;
        data = NULL;
        return;
    }
    file = Strdup(f);
    dir  = dirname(f);

    data = new DataFile(*in);
    base  = Strdup("");
    units = Strdup("");   
}

DBvars::~DBvars()
{
    delete [] units;
    delete [] base;
    if( data )
    {
        ifstream &in = data->in;
        in.close();
        delete &in;
    }
    delete data;
    delete [] dir;
    delete [] file;
}
