#include <cmath>
#include <cstring>
#include <iostream>
using namespace std;

#include "Calc.h"


static int StrCmp(const char *op, const char *ptr)
{
	int i;
	for( i=0; op[i] != '\0'; i++ )
	{
	    if( op[i] != ptr[i] )
	    {
		i = 0;
		break;
	    }
	}
	return i;
}

void Scanner::Init(const char *input)
{
	ptr = line = input;
	current.SetEmpty();
}

int Scanner::NextToken()
{
	if( line == NULL )
	{
		current.SetError();
		return 0;
	}
// skip white space	
	int c;
	for( c=*ptr; ; c=*++ptr )
	{
		if( c == 0 )
		{
			current.SetEof();
			return 0;
		}
		if( !isspace(c) )
			break;	
	}	
	int i;
	const char **ops;
// check for logical operator
	ops = Token::logic_ops;
	for( i=0; (ops)[i] != NULL; i++ )
	{
		int j = StrCmp(ops[i],ptr);
		if( j>0 )
		{
			ptr += j;
			current.SetLogical(ops[i]);
			return 1;
		}
	}
// check for Boolean operator
	ops = Token::bool_ops;
	for( i=0; (ops)[i] != NULL; i++ )
	{
		int j = StrCmp(ops[i],ptr);
		if( j>0 )
		{
			ptr += j;
			current.SetBoolean(ops[i]);
			return 1;
		}
	}
// check for arithmetic operator
	ops = Token::ops;
	for( i=0; (ops)[i] != NULL; i++ )
	{
		int j = StrCmp(ops[i],ptr);
		if( j>0 )
		{
			ptr += j;
			current.SetOperator(ops[i]);
			return 1;
		}
	}
// check for variable and function
	const char *ptr0 = ptr;
	if( isalpha(c) )
	{
		for( ;(c=*ptr) != 0; ptr++ )
		{
			if( !(isalnum(c) || c == '_' || c == '.' ) )
				break;
		}
		if( *ptr == '(' )
		{
			current.SetFunction(ptr0, ptr-1);
			ptr++;
		}
		else
		{
			if( ptr-ptr0 == 3 && !strncmp("and",ptr0,3) )
			{
			    current.SetBoolean("&&");
			}
			else if( ptr-ptr0 == 2 && !strncmp("or",ptr0,2)  )
			{
			    current.SetBoolean("||");
			}
			else
			    current.SetVariable(ptr0, ptr-1);
		}
		return 1;
	}
// check for number
	else if( isdigit(c) || c == '.' )
	{
		double v = strtod(ptr0, (char**)&ptr);
		if( ptr != ptr0 )
		{
			current.SetNumber(v);
			return 1;
		}
	}
// check for literal
    else if( c == '"' || c == '\'' )
    {
        for( ++ptr; *ptr != '\0'; ++ptr )
        {
            if( *ptr == c )
            {
			    current.SetLiteral(ptr0+1, ptr-1);
                ++ptr;
                return 1;
            }
        }
    }
	
	current.SetError();
	return 0;
}

ostream &Scanner::PrintMarker(ostream &out)
{
	if( line == NULL )
	    return out;
	out << line << "\n";
	const char *c = line;
	while( ++c < ptr )
	   out << " ";
	out << "^" << "\n";
	return out;	
}

