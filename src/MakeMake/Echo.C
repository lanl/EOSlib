#include <iostream>
#include <string>
#include <cstring>
using namespace std;

// Echo

char Output(char *s)
{
	char sep;
	
	while(*s)
	{
		sep = ' ';
		
		if(*s == '\\')
		{
			s++;
			if(*s == 'n')
			{
				cout.put('\n');
				sep = 0;
			}
			else if(*s == 't')
			{
				cout.put('\t');
				sep = 0;
			}
			else if(*s == 'r')
			{
				cout.put('\r');
				sep = 0;
			}
			else if(*s == ' ')
			{
				cout.put(' ');
				sep = 0;
			}
			else
			{
				cout.put('\\');
				cout.put(*s);
			}
			s++;
		}
		else
		{
			cout.put(*s++);
		}
	}
	
	return sep;
}




int main(int, char **argv)
{
	char *prog = *argv++;
	char separator;
	
	int newline_flag = 1;
	
	
	if(!*argv)
	{
		cout.put('\n');
		return 0;
	}
	
	if(!strcmp(*argv, "-n"))
	{
		argv++;
		newline_flag = 0;
		
		if(!*argv)
			return 0;
	}
	
	
	while(1)
	{
		separator = Output(*argv++);
		
		if(*argv)
		{
			if(separator)
				cout.put(separator);
		}
		else
		{
			break;
		}
	}
		
	if(newline_flag)
		cout.put('\n');
		
	return 0;
}
