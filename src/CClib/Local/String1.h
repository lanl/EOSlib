#ifndef CCLIB_STRING_H_
#define CCLIB_STRING_H_

#include <cstring>
char *Strdup(const char *s);
/***
 * Equivalent to
inline char* Strdup(const char *str)
{
    return (str==NULL) ? NULL : strdup(str);
}
 * str2 = Strdup(str);
 * free storage with
 * delete []str2;
***/

/***
 * Cat is deprecated
 * use string class
***/
#define MAX_ARGS 16
char *_Cat(const char* ...);	// Allows for upto MAX_ARGS arguments

// #define Cat(...) _Cat(__VA_ARGS__, NULL)
/******/
inline char *Cat(const char *str1)
	{ return Strdup(str1); }
inline char *Cat(const char *s1, const char *s2)
	{ return _Cat(s1, s2, NULL); }
inline char *Cat(const char *s1, const char *s2, const char *s3)
	{ return _Cat(s1, s2, s3, NULL); }
inline char *Cat(const char *s1, const char *s2, const char *s3, const char *s4)
	{ return _Cat(s1, s2, s3, s4, NULL); }
inline char *Cat(const char *s1, const char *s2, const char *s3, const char *s4, const char *s5)
	{ return _Cat(s1, s2, s3, s4, s5, NULL); }
inline char *Cat(const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6)
	{ return _Cat(s1, s2, s3, s4, s5, s6, NULL); }
/******/


class SplitString
{
private:
	char *string;
	char *next;	
public:
	SplitString(const char* s) : string(Strdup(s)) {next = string;}
	~SplitString() { delete [] string; }

    int match;
	const char *WordTo(const char *sep);
	const char *Remainder() {return next;}
};

#endif // CCLIB_STRING_H_
