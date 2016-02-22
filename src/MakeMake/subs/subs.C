#include <cstring>
#include "subs.h"


int GetString(istream& line, const char *term, char* str, char& end_char)
{
	char *s_ptr;
	const char *t_ptr = "";
	
	if( line.eof() )
	{
		str[0] = 0;
		end_char= 0;
		return -1;
	}
	
	for(s_ptr = str; line.get(*s_ptr), line; s_ptr++)
	{
		for(t_ptr = term; *t_ptr; t_ptr++)
			if(*t_ptr == *s_ptr)
				break;
		
		if(*t_ptr)
			break;
	}
	
	end_char = *t_ptr;
	*s_ptr = 0;
	
	return (int)(s_ptr - str);
}


int GetToken(istream& line, const char *term, char* str, char& end_char)
{
	char ch;
	char term2[max_string];
	
	strcpy(term2, " \t");
	strcat(term2, term );

	SkipWhite(line);
		
	int retval = GetString(line, term2, str, end_char);
	
	if(end_char == ' ' || end_char == '\t')
	{
		SkipWhite(line);
		line.get(ch);
		
		if(!line)
		{
			end_char = 0;
			return retval;
		}
		
		for( ; *term; term++)
		{
			if(ch == *term)
				break;
		}
		
		if(*term)
			end_char = *term;
		else
			line.putback(ch);
	}
	
	return (retval < 0) ? 0 : retval;
}


int SkipWhite(istream& s)
{
	char ch;
	int count = 0;
	
	while(s.get(ch), s)
	{
		if(!isspace(ch))
		{
			s.putback(ch);
			break;
		}
		count++;
	}
	
	return count;
}

	
int copywhite(istream &in, ostream &out)
{
	char c;
	while( in.get(c) )
	{
		if( isspace(c) )
			out.put(c);
		else
		{
			in.putback(c);
			return 1;
		}
	}
	return 0;
}

char Suffix(char *str, int len)
{
	if(len < 0)
		len = strlen(str);
	
	if(len - 2 > 0 && str[len-2] == '.')
		return str[len-1];
		
	return 0;
}
	
	


int GetLine(istream& in, char *&oline, int comment)
{
	int cr = 0;
		
	char line[max_string];
	string out;
	
	while( (in.getline(line, max_string-1, '\n')) )
	{
		cr++;
		//in.get();		// eat '\n'
		
		int len = comment ? strlen(line): RemoveComment(line);
		
		if(!len)
			break;
		
		// Note: This convention will continue a line even after
		//	 a comment.
	
		if(line[len-1] == '\\')
		{
			if(comment)
			{
				out = out + line + "\n";
			}
			else
			{
				line[len-1] = 0;
				out = out + line + " ";
			}
		}
		else
		{
			out += line;
			break;
		}
	}

	oline = out.empty() ? NULL : strdup(out.c_str());
	
	if(!oline)
		return 0;
	
	if(!comment && *oline == 0)
	{
		delete oline;
		oline = 0;
	}
	
	return cr;
}


char *istream2string(istream& s)
{
	char ch;
	string out;
	
	while( s.get(ch) )
		out += ch;
			
	return strdup(out.c_str());
}
	

int RemoveComment(char *line)
{
	char *ptr;
	
	for(ptr=line; *ptr; ptr++)
	{
		if(*ptr == '#')
		{
			*ptr = 0;
			break;
		}
	}
	
	return (int)(ptr - line);
}



int FindInclude(ifstream& F, char *name)
{
	const char *Include="#include";
	char c;
	int i;
	
	
	while(F.get(c))
	{
		for(i=0; i < 8; i++)
		{
			if(c == Include[i])
				F.get(c);
			else
				break;
		}
		
		if(c == '\n')
			continue;
			
		else if(i == 8)
		{
			F >> name;
			
			if(!F.good())
				return -1;
				
			while(F && F.get(c), c != '\n')
				;
				
			char *ptr;
			
			if(name[0] == '"')
			{
				ptr = name+1;
				while(*ptr && *ptr != '"')
				{
					ptr[-1] = ptr[0];
					ptr++;
				}
				
				if(!(*ptr))
					return -1;
					
				ptr[-1] = 0;
				
				return 1;
			}
			else if(name[0] == '<')
			{
				ptr = name+1;
				while(*ptr && *ptr != '>')
				{
					ptr[-1] = ptr[0];
					ptr++;
				}
				
				if(!(*ptr))
					return -1;
					
				ptr[-1] = 0;
				return 2;
			}
		}
		else
			while(F && c != '\n')
				F.get(c);
	}
	
	return 0;
}

int Blank(char *str)
{
	for( ; *str; str++)
	{
		if( str[0] == '\\' && str[1] == '\n')
			str++;
		else if( !isspace(*str) )
			return 0;
	}
	return 1;
}



int FindRepeat(const char *line, char m)
{
	const char *c;
	
	for(c=line; *c; c++)
	{
		if( *c =='[' )
		{
			if( c[1] == m && c[2] == ']' )
				return c-line;
		}
	}
	return -1;
}


int FindRepeat(const char *line, const char* m)
{
	const char *c;
	
	for(c=line; *c; c++)
	{
		if( *c =='[' )
		{
			if(c[1] && c[2] != ']')
				continue;
				
			const char *ptr;
			for(ptr = m; *ptr; ptr++)
			{
				if( c[1] == *ptr )
					return c-line;
			}
		}
	}
	return -1;
}
