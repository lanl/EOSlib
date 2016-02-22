#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
#include "DataBase.h"

Property::Property() : type(NULL), params(NULL), obj(NULL)
{
    // Null
}

Property::~Property()
{
    if( obj )
        type->base->Destructor(obj);
}

//

Type::Type() : type(NULL), info(NULL), base(NULL), Constructor(NULL)
{
    // Null
}

Type::Type(const char *tname) : info(NULL), Constructor(NULL)
{
    type = Strdup(tname);
}

Type::~Type()
{
    delete [] type;
    delete [] info;
}

//

Base::Base() : base(NULL), info(NULL), Duplicate(NULL), Initialize(NULL)
{
    // Null
}

Base::Base(const char *bname) : info(NULL), Initialize(NULL), Duplicate(NULL),
                                Destructor(NULL)
{
    base = Strdup(bname);    
}

Base::~Base()
{
    delete [] base;
    delete [] info;
}

//

File::File() : file(NULL)
{
    // Null
}

File::File(const char *fname)
{
    file = Strdup(fname);
}

File::~File()
{
    delete [] file;
}

