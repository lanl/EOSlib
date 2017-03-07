#include "String1.h"

// same as strdup, but use new & delete[] instead of malloc & free
char *Strdup(const char *s)
{
	if( s == NULL )
		return NULL;
	int n = strlen(s) +1;
	char *str = new char[n];
	(void) memcpy(str, s, n*sizeof(char));
	return str;
}
