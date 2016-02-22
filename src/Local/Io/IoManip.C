#include <cstring>
#include <LocalIo.h>
//
// kludge to initialize externs for main program linked to shared object
//
static int_iomanip _EExit   = int_iomanip(ErrorExit, -1);
static str_iomanip _Error  = str_iomanip(ErrorMess);
static str_iomanip _SkipTo = str_iomanip(skipto);
static str_ostream_iomanip _SkipToAndCopy
                    = str_ostream_iomanip(skipto_and_copy, 0, cerr);

int_iomanip &Exit   = _EExit;
str_iomanip &Error  = _Error;
str_iomanip &SkipTo = _SkipTo;
str_ostream_iomanip &SkipToAndCopy = _SkipToAndCopy;
int LocalIo_init = 0;

int LocalIo_Init()
{
    if( LocalIo_init == 0 )
    {
        Exit   = *(new int_iomanip(ErrorExit, -1));
        Error  = *(new str_iomanip(ErrorMess));
        SkipTo = *(new str_iomanip(skipto));
        SkipToAndCopy = *(new str_ostream_iomanip(skipto_and_copy, 0, cerr));
        LocalIo_init = 1;
    }
    return 1;
}
//
//
//
str_iomanip::str_iomanip(void (*F)(istream&, const char *), const char *str)
{
	ifunc = F;
	ofunc = 0;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
}



str_iomanip::str_iomanip(void (*F)(ostream&, const char *), const char *str)
{
	ofunc = F;
	ifunc = 0;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
}



str_iomanip::str_iomanip(void (*Fin)(istream&, const char *),
			void (*Fout)(ostream&, const char *), const char *str)
{
	ifunc = Fin;
	ofunc = Fout;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
}


str_iomanip& str_iomanip::operator()(const char *str)
{
	if(value)
		delete [] value;
	value = str ? new char[strlen(str) +1] : 0;
	
	if(value)
		strcpy(value, str);
		
	return(*this);
}

str_ostream_iomanip::str_ostream_iomanip(void (*F)(istream&, const char *, ostream&),
						const char *str, ostream& s)
{
	ifunc = F;
	ofunc = 0;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
	stream = &s;
}



str_ostream_iomanip::str_ostream_iomanip(void (*F)(ostream&, const char *, ostream&),
						const char *str, ostream& s)
{
	ofunc = F;
	ifunc = 0;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
	stream = &s;
}



str_ostream_iomanip::str_ostream_iomanip(void (*Fin)(istream&, const char*, ostream&),
		void (*Fout)(ostream&, const char *, ostream&), const char *str, ostream& s)
{
	ifunc = Fin;
	ofunc = Fout;
	value =  str ? new char[strlen(str)+1] : 0;
	if(value)
		strcpy(value, str);
	stream = &s;
}


str_ostream_iomanip& str_ostream_iomanip::operator()(const char *str, ostream& s)
{
	if(value)
		delete [] value;
	value = str ? new char[strlen(str) +1] : 0;
	
	if(value)
		strcpy(value, str);
		
	stream = &s;
		
	return(*this);
}
