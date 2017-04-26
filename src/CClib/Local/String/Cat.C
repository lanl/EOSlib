#include "String1.h"
#include <stdarg.h>

char *_Cat(const char *arg0, ...)
{
	if( arg0 == NULL )
	    return NULL;

	const char **list = new const char*[MAX_ARGS];
	list[0] = arg0;
	int n = 1;
	int len = strlen(arg0);
	
	va_list ap;
	va_start(ap,arg0);
	const char *arg;
	while( (arg = va_arg(ap, char*)) )
	{
	    if( n == MAX_ARGS )
                break;		// ToDo realloc or report error
	    list[n++] = arg;
	    len += strlen(arg);
	}
	va_end(ap);

	char *ans = new char[len+1];
	int c = 0;
	int i;
	for( i=0; i<n; i++ )
	{
	    for( arg=list[i]; *arg != '\0'; arg++ )
	        ans[c++] = *arg;
	}
	ans[c] = '\0';
	return(ans);
}


