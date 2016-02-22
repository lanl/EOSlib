#include <cstring>
#include "DataFile.h"


#define LINE_DIM 1024
Line::Line(int Dim) : dim(Dim), n(0)
{
    if( dim <= 0 )
        dim = LINE_DIM;
	line = new char[dim];
}

Line::~Line()
{
	delete [] line;
}

void Line::ReAlloc()
{
	int dim0 = dim;
	char *line0 = line;
	dim *= 2;
	line = new char[dim];
	memcpy(line,line0, sizeof(char)*dim0);
	delete [] line0;	
}

void Line::Add(char c)
{
	if( n+1 == dim )
		ReAlloc();
	line[n++] = c;
	line[n] = '\0';
}

char *Line::operator[](int i)
{
	return (i<n) ? line + i : NULL;
}

