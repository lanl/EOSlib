#include <cstring>
#include "Calc.h"

const char *Token::logic_ops[] = {"<=", "==", "!=", ">=", "<", ">", NULL};
const char *Token::bool_ops[]  = {"||", "&&", "!", NULL};
const char *Token::ops[] = {"+=", "-=", "/=", "*=", "=", "+", "-", "*", "/", "^",
                       "(",  ")",  ",",  "?", ":", NULL};

Token::~Token()
{
	delete [] string;
}

void Token::Set(const char *word, double v)
{
	delete [] string;
	string = (word==NULL) ? NULL : strdup(word);	
	value = v;
}

void Token::Set(const char* ptr0, const char *ptr1, double v)
{
	delete [] string;
	
	if( ptr0 == NULL )
		string == NULL;
	else
	{
		int n=ptr1-ptr0 + 1;
		string = new char[n+1];
		int i;
		for(i=0; i<n; i++)
			string[i] = ptr0[i];
		string[n] = '\0';
	}
	
	value = v;
}

