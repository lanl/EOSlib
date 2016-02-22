#ifndef CCLIB_LOCAL_IO_
#define CCLIB_LOCAL_IO_

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <iomanip>
using namespace std;


// class declarations:

class int_iomanip;
class str_iomanip;
class str_ostream_iomanip;

// function declarations:

void ErrorExit(ostream& out, int status);
char* ProgName(const char *name);
void ErrorMess(ostream& out, const char *name);
void skipto(istream& inp, const char* string);
void skipto_and_copy(istream& inp, const char* string, ostream& s);



// Global Variables


extern int_iomanip &Exit;
extern str_iomanip &Error;
extern str_iomanip &SkipTo;
extern str_ostream_iomanip &SkipToAndCopy;
extern int LocalIo_init;

int LocalIo_Init();
// kludge to initialize externs for main program linked to shared object
static int init_so = LocalIo_Init();

// Enumerations

enum FormatFlag { BINARY, ASCII };

// Definitions:

class int_iomanip
{
	void (*ofunc)(ostream&, int);
	void (*ifunc)(istream&, int);
	int value;
public:
	int_iomanip ( void (*F)(ostream&, int), int i = 0)
					{ofunc = F; ifunc = 0; value = i;}
						
	int_iomanip ( void (*F)(istream&, int), int i = 0)
					{ifunc = F; ofunc = 0; value = i;}
						
	int_iomanip ( void (*Fin)(istream&, int), void (*Fout)(ostream&, int),
								 int i = 0)
					{ifunc = Fin; ofunc = Fout; value = i;}
						
	int_iomanip& operator()(int arg) {value = arg; return *this;}

	friend ostream& operator<<(ostream& out,  int_iomanip man)
			{if(man.ofunc) man.ofunc(out, man.value); return out;}

	friend istream& operator>>(istream& inp,  int_iomanip man)
			{if(man.ifunc) man.ifunc(inp, man.value); return inp;}
};


class str_iomanip
{
	void (*ifunc)(istream&, const char*);
	void (*ofunc)(ostream&, const char*);
	char* value;
public:
	str_iomanip(void (*F)(ostream&, const char *), const char *str = 0);
	str_iomanip(void (*F)(istream&, const char *), const char *str = 0);
	str_iomanip(void (*F)(istream&, const char *),
			void (*G)(ostream&, const char *), const char *str = 0);
	~str_iomanip() { /* if(value) delete [] value; */ }
	str_iomanip& operator()(const char *);
	friend istream& operator>>(istream& inp,  str_iomanip man)
			{if(man.ifunc) man.ifunc(inp, man.value); return inp;}
	friend ostream& operator<<(ostream& out,  str_iomanip man)
			{if(man.ofunc) man.ofunc(out, man.value); return out;}
};



class str_ostream_iomanip
{
	void (*ifunc)(istream&, const char*, ostream&);
	void (*ofunc)(ostream&, const char*, ostream&);
	char* value;
	ostream* stream;
public:
	str_ostream_iomanip(void (*F)(ostream&, const char*, ostream&),
						const char*, ostream&);
	str_ostream_iomanip(void (*F)(istream&, const char*, ostream&),
						const char*, ostream&);
	str_ostream_iomanip(void (*F)(istream&, const char*, ostream&),
		void (*G)(ostream&,const char*,ostream&),const char*,ostream&);
	str_ostream_iomanip& operator()(const char *, ostream&);
	friend istream& operator>>(istream& inp,  str_ostream_iomanip man)
	     {if(man.ifunc) man.ifunc(inp, man.value, *man.stream); return inp;}
	friend ostream& operator<<(ostream& out,  str_ostream_iomanip man)
	     {if(man.ofunc) man.ofunc(out, man.value, *man.stream); return out;}
};

#endif  /* CCLIB_LOCAL_IO_ */
