#ifndef CCLIB_LOCAL_IO_
#define CCLIB_LOCAL_IO_

//#include <cstdlib>
//#include <cstddef>
#include <iostream>
#include <iomanip>
//using namespace std;


// class declarations:

class int_iomanip;
class str_iomanip;
class str_ostream_iomanip;

// function declarations:

void ErrorExit(std::ostream& out, int status);
char* ProgName(const char *name = nullptr);
void ErrorMess(std::ostream& out, const char *name);
void skipto(std::istream& inp, const char* string);
void skipto_and_copy(std::istream& inp, const char* string, std::ostream& s);



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
	void (*ofunc)(std::ostream&, int);
	void (*ifunc)(std::istream&, int);
	int value;
public:
	int_iomanip ( void (*F)(std::ostream&, int), int i = 0)
					{ofunc = F; ifunc = 0; value = i;}
						
	int_iomanip ( void (*F)(std::istream&, int), int i = 0)
					{ifunc = F; ofunc = 0; value = i;}
						
	int_iomanip ( void (*Fin)(std::istream&, int), void (*Fout)(std::ostream&, int),
								 int i = 0)
					{ifunc = Fin; ofunc = Fout; value = i;}
						
	int_iomanip& operator()(int arg) {value = arg; return *this;}

	friend std::ostream& operator<<(std::ostream& out,  int_iomanip man)
			{if(man.ofunc) man.ofunc(out, man.value); return out;}

	friend std::istream& operator>>(std::istream& inp,  int_iomanip man)
			{if(man.ifunc) man.ifunc(inp, man.value); return inp;}
};


class str_iomanip
{
	void (*ifunc)(std::istream&, const char*);
	void (*ofunc)(std::ostream&, const char*);
	char* value;
public:
	str_iomanip(void (*F)(std::ostream&, const char *), const char *str = 0);
	str_iomanip(void (*F)(std::istream&, const char *), const char *str = 0);
	str_iomanip(void (*F)(std::istream&, const char *),
			void (*G)(std::ostream&, const char *), const char *str = 0);
	~str_iomanip() { /* if(value) delete [] value; */ }
	str_iomanip& operator()(const char *);
	friend std::istream& operator>>(std::istream& inp,  str_iomanip man)
			{if(man.ifunc) man.ifunc(inp, man.value); return inp;}
	friend std::ostream& operator<<(std::ostream& out,  str_iomanip man)
			{if(man.ofunc) man.ofunc(out, man.value); return out;}
};



class str_ostream_iomanip
{
	void (*ifunc)(std::istream&, const char*, std::ostream&);
	char* value;
	std::ostream* stream;
public:
	str_ostream_iomanip(void (*F)(std::istream&, const char*, std::ostream&),
						const char*, std::ostream&);
	str_ostream_iomanip& operator()(const char *, std::ostream&);
	friend std::istream& operator>>(std::istream& inp,  str_ostream_iomanip man)
	     {if(man.ifunc) man.ifunc(inp, man.value, *man.stream); return inp;}
};

#endif  /* CCLIB_LOCAL_IO_ */
