#include <iomanip>
#include "DataFile.h"
using namespace std;
#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}


int IsBlank(const char *str)
{
    for( ; *str != '\0'; str++ )
    {
        if( *str != ' ' && *str != '\t' )
            return 0;
    }
    return 1;
}

char *DeleteSpace(char *str, int flag)
{
// flag &= 1 lead space
//      &= 2 trailing space
//      >3 all blank
    if( str == NULL )
        return NULL;
    if( flag>3 || flag&1 )
    {
        while( *str == ' ' || *str == '\t' )
            ++str;
    }
    if( flag > 3 )
    {
        int n=0;
        char *c;
        for(c=str ;*c != '\0'; c++)
            if( *c != ' ' && *c != '\t' )
                str[n++] = *c;
        str[n] = '\0';
    }
    else if( flag&2 )
    {
        int n = strlen(str)-1;
        while( n>=0 && (str[n]==' '|| str[n]=='\t') )
            str[n--] = '\0';
    }
    return str;
}


static int SkipToEndComment(istream &in, int &lines)
{
    int c;
    while( (c=in.get()) != EOF )
    {
        while( c == '*' )
        {
            c = in.get();
            if( c == '/' )
                return 0;
            else if( c != '*' )
                break;    
        }
        if( c == '\n' )
            lines++;    
    }
    return 1;
}

int DataFile::NextLine()
{
    line.Reset();
    last_word = NULL;
    word = NULL;
    chars_read = 0;
    words = 0;
    if( !in )
        return 0;
        
    char last = '\0';
    int c;
    while( (c=in.get()) != '\n' )
    {
        if( c == EOF )
            return 0;

        if( c == '/' )
        {
            if( in.peek() == '*' )
            {
                c = in.get();
                if( SkipToEndComment(in, lines) )
                {
                    status = Strdup("SkipToEndComment failed");
                    return 0;
                }
                chars_read++;
                continue;
            }
        }
        chars_read++;

        if( c == '#' )
        {    // comment, skip rest of line
            do
            {
                c = in.get();
            } while( c != EOF && c != '\n' );
            break;
        }
        else if( c == ':' )
        {
            if( isspace(in.peek()) )
            { // comment out beginning of line
                line.Reset();
                words = 0;
                last = '\0';
                continue;
            }
        }
        /****
        else if( c == ' ' || c == '\t' )
        {
            if( last == ' ' )
                continue;
            c = ' ';
        }
        ***/
        else if( c == ';' || c == '{' || c == '}' )
        {
            if( last != '\0' )
            {
                line.Add('\0');
                words++;
            }
            if( c == '{' || c == '}' )
            {
                line.Add(c);
                line.Add('\0');
                words++;
            }
            last = '\0';
            continue;
        }
        last = c;
        line.Add(c);
    }
    
    if( last != '\0' )
    {
        line.Add('\0');
        words++;
    }
    else if( words == 0 )
        line.Add('\0');

    word = line[0];
    lines++;
    return 1;
}

char *DataFile::NextWord(int online, int delete_space)
{
    do {
        while( words == 0 )
        {
            last_word = NULL;
            if( online )
                return NULL;
            if( NextLine() == 0 )
                return NULL;
        }
        last_word = word;
        words--;
        word += strlen(word)+1;
        if( delete_space )
        {
            if( (last_word=DeleteSpace(last_word)) )
                if( last_word[0] == '\0' )
                    last_word = NULL;
        }
    } while( last_word == NULL && delete_space == 2);
    return last_word;
}

char *DataFile::LastWord(int delete_space)
{
    if( delete_space )
        last_word = DeleteSpace(last_word);
    return last_word;
}

