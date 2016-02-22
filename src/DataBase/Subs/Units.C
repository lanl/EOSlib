#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
#include "DataBase.h"

const char *Units::fundamental[] = {"L", "t", "m", "T", 0};

int IsFundamental(const char *name)
{
    const char **u;
    for(u=Units::fundamental; *u != NULL; u++)
    {
        if( !strcmp(*u,name) )
            return 1;
    }
    return 0;
}

//

int deleteUnits(Units *&units)
{
    int status;
    if( units == NULL )
        status = -1;
    else if( units->ref_count == 1 )
    {
        delete units;
        status = 0;
    }
    else
    {
        status = --(units->ref_count);
        if( status > 1000 || status < 0 )
        {
            // deallocation error, throw exception
        }
    }
    units = NULL;
    return status;
}

extern "C" {
  int  Units_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      return ((Units *)ptr)->Initialize(p,db);
  }
  
  void *Units_Duplicate(void *ptr)
  {
      return (void *)((Units *)ptr)->Duplicate();
  }
      
  void Units_Destructor(void *ptr)
  {
      Units *uptr  = (Units *)ptr;
      deleteUnits(uptr);
  }
  
  void *Units_Units_Constructor()
  {
      return new Units;
  }
}

//
//
Units::Units(const char *type, const char *name,
             Parameters *&Values,Parameters *&Names)
                    : values(Values), names(Names)
{
    Values = Names = NULL;
    utype = Strdup(type);
    uname = Strdup(name);
}

Units::~Units()
{
    if( --ref_count )
        DB_error->Abort("Units::~Units", __FILE__, __LINE__, NULL,
               "ref_count=%d, allocation error\n\t%s:%s\n",
               ref_count,utype,uname);

    delete [] utype;
    delete [] uname;
    delete values;
    delete names;
}

Units *Units::Duplicate()
{
    ref_count++;
    return this;
}

const char *Units::Unit(const char *name) const
{
    const char *u = names ? names->Value(name) : NULL;
    return  u ? u : "?";
}

const char *Units::Value(const char *name) const
{
    const char *v = values ? values->Value(name) : NULL;
    return  v ? v : "?";
}

int Units::Match(const char *type_name) const
{
    const char *type;
    const char *name;
    if( TypeName(type_name,type,name) )
        return 0;
    int ans = Match(type,name);
    delete [] type;
    delete [] name;
    return ans;
}

