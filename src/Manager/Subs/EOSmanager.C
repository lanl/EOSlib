#include "EOSmanager.h"
#include "SplitWord.h"

#include <iostream>
#include <iomanip>
using namespace std;

int EOS_Array::Find(const char *ename, const char *uname)
{
    if( ename == NULL )
        return -1;

    if( uname == NULL )
        return _PointerArray::Find(ename);

    int i;
    for( i=0; i<n; i++ )
    {
         if( !strcmp(ename,names[i]) )
         {
             EOS *eos = (*this)[i];
             if( !strcmp(uname,eos->UseUnits()->Name()) )
                 return i;
         }
    }
    return -1;
}

EOS_Array::~EOS_Array()
{
    int i;
    for( i=0; i<n; i++)
    {
	     delete [] names[i];
	     EOS *eptr = static_cast<EOS*>(pointers[i]);
	     deleteEOS(eptr);
    }
    n = 0;
}

Units_Array::~Units_Array()
{
    int i;
    for( i=0; i<n; i++)
    {
	     delete [] names[i];
	     Units *uptr = static_cast<Units*>(pointers[i]);
	     deleteUnits(uptr);
    }
    n = 0;    
}
    
EOSmanager::EOSmanager(ostream &log) : env(NULL), data_files(NULL), EOSlog(log)
{
    EOS::Init();
    DataBase::error_default = new DataBase_Error(log);
     EOSbase::ErrorDefault  = new EOSbase::Error(log);
   status = EOSlib::fail;
}

EOSmanager::~EOSmanager()
{
    delete [] env;
    delete [] data_files;
}

void EOSmanager::SetEnv(const char *Env)
{
   Parameters &params = *GetProp(*database,"Environment").params;
   SplitWord EnvVar(Env);
   while( EnvVar.Next() )
      params.Add(EnvVar.param, EnvVar.value);
   if( env && env[0] != '\0' )
   {
       char *tmp = env;
       env = Cat(env, ":", Env);
       delete [] tmp;
   }
   else
   {
       delete [] env;
       env = Strdup(Env);
   }
}

int EOSmanager::Read(const char *files)
{
   if( DataBase::Read(files) )
   {
       status = EOSlib::fail;
       return 1;
   }
   else
   {
       if( data_files && data_files[0] != '\0' )
       {
           char *tmp = data_files;
           data_files = Cat(data_files, ":", files);
           delete [] tmp;
       }
       else
       {
           delete [] data_files;
           data_files = Strdup(files);
       }
       status = EOSlib::good;
       return 0;
   }
}


int EOSmanager::index_eos(const char *ename, const char *uname)
{
    if( ename == NULL || uname == NULL )
    {
        EOSlog << "Error, EOSmanager::Index, ename or uname is NULL\n";
        return -1;
    }
    
    int i = e.Find(ename, uname);
    if( i >= 0 )
        return i;
   
    int u_i = index_units(uname);
    if( u_i < 0 )
    {
        EOSlog << "Error, EOSmanager::Index(" << ename << "," << uname << "), "
               << "index_units failed\n";
        return -1;
    }

    EOS *ptr =  FetchNewEOS(ename, *this);
    if( ptr == NULL )
    {
        EOSlog << "Error, EOSmanager::Index(" << ename << "," << uname << "), "
               << "FetchEOS failed\n";
        return -1;
    }
    if( ptr->ConvertUnits(*u[u_i]) )
    {
        EOSlog << "Error, EOSmanager::Index(" << ename << "," << uname << "), "
               << "ConvertUnits failed\n";
        return -1;
    }
    EOS &E = *ptr;
    i = e.Add(ename,ptr);
    
    EOSlog << "Index " << i << " added to EOS list\n";
    E.Print(EOSlog) << endl;
    
    return i;
}

int EOSmanager::index_units(const char *name)
{
    if( name == NULL )
        return -1;
    
    int i = u.Find(name);
    if( i >= 0 )
        return i;
    
    char *uname = Cat("hydro::",name);
    Units *ptr =  FetchUnits(uname);
    delete [] uname;
    if( ptr == NULL )
    {
        EOSlog << "Error, EOSmanager::index_units(" << name << "), "
               << "FetchUnits failed\n";
        return -1;
    }
    Units &U = *ptr;
    i = u.Add(name,ptr);
    
    EOSlog << "Index " << i << " added to Units list\n";
    U.Print(EOSlog) << endl;
    
    return i;
}
