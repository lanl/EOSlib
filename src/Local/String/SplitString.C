#include "String.h"
#include <iostream>
using namespace std;

static int Match(const char *sep, const char *word)
{
    int n;
    for( n=0; sep[n] != '\0'; n++)
    {
        if( word[n] == '\0' )
            return -1;
        if( sep[n] != word[n] )
            return 0;
    }
    return n;
}

const char *SplitString::WordTo(const char *sep)
{
    match = 0;
    if( next == NULL )
        return NULL;
    if( sep == NULL )
    {
        char *retval = next;
        next = NULL;
        return retval;
    }
    
    char *word = next;
    for( ; *next != '\0'; next++ )
    {
        int n = Match(sep,next);
        if( n == -1 )
        {  // string ended with no match
            next = NULL;
            break;
        }
        else if( n > 0 )
        {  // match
            match = 1;
            *next = '\0';
            next += n;
            break;
        }
    }
    if( next && *next == '\0')
        next = NULL;
    
    return word;
}

