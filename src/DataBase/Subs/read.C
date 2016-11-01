#include <String1.h>     // SplitString
#include "DataBase.h"

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

int DataBase::OpenFile(const char *file)
{
	if( file == NULL || file[0] == '\0' )
    {
        error->Log("DataBase::OpenFile", __FILE__, __LINE__, this,
                   "file = NULL\n" );
        return 1;
    }
    DBvars *v = new DBvars(file,vars);
    if( v->data == NULL )
    {
        error->Log("DataBase::OpenFile", __FILE__, __LINE__, this,
                   "could not open file `%s'\n", file );
        delete v;
        return 1;
    }
	Files[file];
    vars = v;
    return 0;
}
int DataBase::CloseFile()
{
    if( vars == NULL )
        return 0;
    int status = vars->data->in.bad();
    DBvars *v = vars;
    vars = v->prev;
    delete v;
    return status;
}

int DataBase::Read(const char *files)
{
    SplitString list(files);
    const char *file;
    vars = NULL;
    int status = 0;
    while( (file=list.WordTo(":"))!= NULL )
    {
        status=read(file);
        if( status )
            break;
    }
    return status;
}

#define FUNC "DataBase::read", __FILE__, __LINE__, this
int DataBase::read(const char *file)
{
    if( Files.Defined(file) )
        return 0;
    if( OpenFile(file) )
        return 1;
    DataFile &data = *vars->data;
    
    int eq;
    while( data.in )
    {
        if( DotCommands() )
        {
            eq = ERR;
            break;
        }
        if( data.in.eof() )
        {
            eq = END;
            break;
        }
        Parameters *p = new Parameters;
        if( (eq=ReadParameters(*p)) == ERR || StoreParameters(eq,p) )
            break;
    }
    if( data.status )
    {
        eq = ERR;
        error->Log(FUNC,"DataFile %s, error status: %s\n", file, data.status);
    }
    if( eq == ERR )
        error->Log(FUNC, "failed reading file `%s' at line %d\n",
                   file, data.lines);    
    return CloseFile() || eq==ERR;
}

#undef FUNC
#define FUNC "DataBase::StoreParameters", __FILE__, __LINE__, this
int DataBase::StoreParameters(int &eq, Parameters *p)
{
    Property &prop = GetProp(*p);
    switch (eq) {
      case ADD:
        if( prop.params )
        {
            prop.params->Append(*p);
            delete p;
        }
        else
          prop.params = p; 
        break;
      case NEW:
        if( prop.params )
        {
            error->Log(FUNC, "%s:%s::%s already defined\n",
                       p->base, p->type ,p->name);
            eq = ERR;
            delete p;
        }
        else
            prop.params = p;
        break;
      case REPLACE:
        delete prop.params;
        prop.params = p;
        break;
      case EXIST:
        if( prop.params )
            delete p;
        else
          prop.params = p; 
        break;
    }
    return eq == ERR;
}

