#include "SplitWord.h"
#include <cstring>

int SplitWord::Seek(char c)
{
    for( n=0; word[n] != '\0'; n++ )
    {
        if( word[n] == c )
            return 1;
    }
    return 0;
}

char *SplitWord::Copy()
{
    char *str;
    if( n > 0 )
    {
       str = new char[n+1];
       int i;
       for(i=0 ; i<n; i++)
         str[i] = word[i];
       str[i] = '\0';
    }
    else
       str = strdup("");

    if( *word == '\0' )
        word = NULL;
    else
        word += n+1;
    return str;
}

int SplitWord::Next()
{
    if( word == NULL )
        return 0;
    delete [] param;  param = NULL;
    delete [] value;  value = NULL;
    if( Seek('=') )
    {
        param = Copy();
        (void) Seek(':');
        value = Copy();    
        return 1;
    }
    else
    {
        word = NULL;
        return 0;
    }
}

