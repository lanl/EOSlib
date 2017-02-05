#include <string>
#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
using namespace std;
#include "../DataBase.h"

/*
Commands before Parameters:
.include <file>
.base =  <class>
.units = type::name
*/
#define FUNC "DataBase::DotCommands", __FILE__, __LINE__, this
int DataBase::DotCommands()
{
    DataFile &data = *vars->data;
    char *s;
    while( (s=data.NextWord(0)) )
    {
        if( *s != '.' )
        {
            if( IsBlank(data.LastWord()) )
                continue;
            else
                break;
        }
        if( !strncmp(s, ".include ",9) )
        {
            int status;
            char *file = DeleteSpace(s+9);
            if( file[0] == '\0' )
            {
                error->Log(FUNC, ".include must be followed by file name\n");
                return 1;
            }
            else if( file[0] == '/' )
                status = read(file);
            else if(  file[0] == '$' )
            {
            	const char *name = EnvironmentVar(file+1);
                if( name == NULL )
                {
                    error->Log(FUNC, ".include failed for file '%s'\n",file);
                    return 1;
                }
                status  = read(name);
            }
            else
            {
                string str(vars->dir);
                str += file;
                status  = read(str.c_str());
            }
            if( status )
            {
                error->Log(FUNC, ".include failed\n");
                return 1;
            }
        }
        else if( !strncmp(s,".base",5) )
        {
            s = DeleteSpace(s+5);
            if( s[0] == '=' )
                s++;
            delete [] vars->base;
            vars->base = Strdup(s);
        }
        else if( !strncmp(s,".units",6) )
        {
            s = DeleteSpace(s+6);
            if( s[0] == '=' )
                s++;
            delete [] vars->units;
            vars->units = Strdup(s);
        }
        else
        {
            error->Log(FUNC, "unknown command %s\n", s);
            return 1;
        }
    }
    return 0;
}
const char *DataBase::EnvironmentVar(const char *file)
{
// file = "env_var/path_name"
// returns string with value of env_var substituted
//         or NULL for error
//  Also adds env_var and its value to :DataBase::Environment
    string str(file);
    size_t n1 = str.find("/");
    if( n1==string::npos || n1==0 || n1==str.size()-1 )
        return NULL;
    string var = str.substr(0,n1);
    Parameters &params = *GetProp(*database,"Environment").params;
    const char* name = var.c_str();
    const char *val = params.Value(name);
    if( val == NULL )
    {
    	if( (val=getenv(name)) )
            params.Append(name,val);
        else
            return NULL;           
    }
    str.replace(0,n1,val);
    return str.c_str();
}
