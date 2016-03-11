#include <LocalIo.h>
#include <iostream>
#include <cstring>
using namespace std;

void ErrorMess(ostream& out, const char *name)
{
	out << "Error in ";
	char *str = ProgName(0);
	if(str)
		out << str;
	out << ": ";
	if(name)
		out << name;
}



void ErrorExit(ostream& out, int status)
{
	if(status)
	{
		out << "\n\t(Exiting with error status "
						<< status <<")\n\n" << flush;
		exit(status);
	}
	out << "\n";
}

void skipto(istream& inp, const char *string)
{
	int c;
	
	if(!string || !(*string))		// No comparison string
		return;
		
	const char *state = string;
	
	while(c = inp.get(), inp)
	{
		if(c == *state)			// c matches comparison
		{
			if(!(*++state))		// c matched last character
				return;
		}
		else if(state != string)	// c failed after partial match
		{
			const char *start, *ptr, *comp;
			
			// Looking for smaller partial match in string
			// E.g.
			for(start = string + 1; start < state; start++)
			{
				ptr = start;
				comp = string;
				

				while(ptr < state && *ptr++ == *comp++)
					;
					
				if(ptr == state && *ptr == c)
					break;
			}
			
			state = (ptr == state && *ptr == c) ? comp++ : string; 
		}
	}
}

void skipto_and_copy(istream& inp, const char *string, ostream& out)
{
	int c;
	
	if(!string || !(*string))		// No comparison string
		return;
		
	const char *state = string;
	
	while(c = inp.get(), inp)
	{
		if(c == *state)			// c matches comparison
		{
			if(!(*++state))		// c matched last character
				return;
		}
		else if(state != string)	// c failed after partial match
		{
			const char *start, *ptr, *comp;
			
			// Looking for smaller partial match in string
			// E.g.
			
			out << string[0];
			
			for(start = string + 1; start < state; start++)
			{
				ptr = start;
				comp = string;
				

				while(ptr < state && *ptr++ == *comp++)
					;
					
				if(ptr == state && *ptr == c)
					break;
					
				out << start[0];
			}
			
			if(ptr == state && *ptr == c)
			{
				state = comp++;
			}
			else
			{
				state = string;
				out << (char)c;
			}
		}
		else
			out << (char)c;
	}
}




char* ProgName(const char *name)
{
  static std::string progname("");
  if(name){ progname = name;}	
  return &(progname[0]);
}
