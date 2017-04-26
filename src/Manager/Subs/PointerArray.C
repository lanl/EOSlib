#include "PointerArray.h"
#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ? NULL : strdup(str);
}

_PointerArray::_PointerArray(int Dim) : dim(Dim), n(0)
{
	if( dim < 4 )
	    dim = 4;
	
	   names = new char*[dim];
	pointers = new void*[dim];
}


void _PointerArray::DeAlloc()
{
	int i;
	for( i=0; i<n; i++ )
	{
	     delete [] names[i];
	     Delete(pointers[i]);
	}
	delete [] names;
	delete [] pointers;
}

void _PointerArray::ReAlloc()
{
	char **names0 = names;
	void **ptr0   = pointers;
	int    dim0   = dim;

	dim *= 2;
	int i;
	for( i=0; i<n; i++ )
	{
	        names[i] = names0[i];
	     pointers[i] =   ptr0[i];
	}
	delete [] names0;
	delete [] ptr0;
}


int _PointerArray::Add(const char *name, void *ptr)
{
	if( n == dim )
	    ReAlloc();

 	   names[n] = Strdup(name);
	pointers[n] = ptr;
	return n++;
}

int _PointerArray::Replace(const char *name, void *ptr)
{
	int n = Find(name);
	if( n >= 0 )
	{
	    Delete( pointers[n] );
	    pointers[n] = ptr;
	}
	else
	{
	    n = Add(name,ptr);
	}
	return n;
}

int _PointerArray::Find(const char *name)
{
	int i;
	for( i=0; i<n; i++ )
	{
	     if( !strcmp(name,names[i]) )
		 return i;
	}
	return -1;
}
