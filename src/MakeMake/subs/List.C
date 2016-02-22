#include <iostream>
using namespace std;
#include <cstdlib>
#include <cstring>

#include "List.h"


List::List(const List&)			// Disallowed
{
	exit(13);
}

void List::operator=(const List&)	// Disallowed
{
	exit(13);
}


List::List(int Max) : dim(Max)
{
	list = new (char *[dim]);
	n = 0;
}

List::~List()
{
	int i;
	
	for( i=0; i<n; i++)
		delete list[i];
	
	delete list;
}

void List::Delete()
{
	int i;
	
	for(i=0; i < n; i++)
		delete list[i];
	
	n = 0;
}


void List::Realloc()
{
	int dim0 = dim;
	char **list0 = list;
	
	dim *= 2;
	list = new (char *[dim]);
	
	int i;
	for( i=0; i < dim0; i++)
		list[i] = list0[i];
	
	delete list0;
}


int List::operator[](const char *str)
{
	int i;
	for( i=0; i< n; i++)
	{
		if( list[i] && !strcmp(str, list[i]) )
			return i;
	}
	return -1;
}

char* List::operator[](int i)
{
	if( i < 0 || n <=i )
	{
		cerr << "List::operator[](int), index out of range\n";
		exit(-1);
	}
	
	return list[i];
}

List& List::operator<<(const char *str)
{
	if( n == dim )
		Realloc();
		
	list[n++] = strdup(str);
	
	return *this;
}


void List::operator=(char *str)
{
	if( n == dim )
		Realloc();
		
	list[n++] = str;
}

void List::Replace(int i, char *s)
{
	if( i < 0 || n <=i )
	{
		cerr << "List::Replace(int, char), index out of range\n";
		exit(-1);
	}
	
	delete list[i];
	list[i] = s;
}
