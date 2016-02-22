#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "Depend.h"
#include "subs.h"

int Dependencies::ProcessTemplate(char *t_file, ostream &out)
{
	Template.open(t_file);
	
	if(!Template.good())
	{
		cerr << "Cannot open Template file\n";
		return -1;
	}

	
	int n = macro["IFLAGS"];
	if( n > -1 && macro_eval[n] )
	{
		char *reformat;
		istringstream iflags(macro_eval[n]);
		ProcessIflags(iflags, reformat);
		macro_eval.Replace(n, reformat);
	}
		
	
	while( Template )
	{
		char *line = GetExpandLine();
		if( !line )
			continue;

		switch(LineType(line))
		{
		case INPUT:
		{
			if( !include )
				break;
				
			char ch;
			while(dep.get(ch))
				out.put(ch);
			break;
		}
		case MACRO_LIST:
		{
			int i;
			for( i=0; i < macro; i++ )
			{
				if( *macro[i] == '\0' )
					continue;
				out << macro[i] << "=" << macro_eval[i] << "\n";
			}
			
			macro.Delete();
			macro_eval.Delete();
			break;
		}
		case INCLUDE_DEPS:
		{
			if(include_dir.Dimension() <= 0)
				include_dir << strdup(".");
		
			Include();
			break;
		}
		case RULE:
		{
			while(1)
			{
				RepeatString(out, line);
				
				if(!Template || Template.peek() != '\t')
					break;
					
				delete line;
				line = GetExpandLine();
			}
			break;
		}
		case REPEAT_RULE:
		{
			char *check0 = strdup(line);
			char *check= check0;
			
			while( *check != ':' )
				 check++;
			(void) RemoveComment(check++);
			
			int offset;
			if( !Repeater(check, 'i', offset) )
				check = 0;
			
			string temp(line);
			temp += "\n";
			while( Template && Template.peek() == '\t')
			{
				delete line;
				line = GetExpandLine();
				temp = temp + line + "\n";
			}
			char *t_out = strdup(temp.c_str());
			
			RepeatString(out, t_out, check);
			delete t_out;
			delete check0;
			break;
		}
		case MACRO:
		{
			char token[max_string];
			char e_c;
			int i;
			istringstream tkstr(line);
			char *eval_str;
	
			GetToken(tkstr, "=", token, e_c);
			
			if((i = macro[token]) < 0 )
			{
				if(!strcmp(token, "IFLAGS"))
				{
					ProcessIflags(tkstr, eval_str);
					out << "IFLAGS="<<eval_str<<"\n";
				}
				else
				{
					out << line << "\n";
					eval_str = istream2string(tkstr);
				}
				
				if( (n = templ_macro[token]) < 0)
				{
					templ_macro << token;
					templ_macro_eval << eval_str;
				}
				else
				{
					templ_macro_eval.Replace(n, eval_str);
				}
			}
			else
			{
				out << macro[i] << "=" << macro_eval[i] << "\n";
				templ_macro << macro[i];
				templ_macro_eval << macro_eval[i];
				macro.Replace(i,strdup(""));
				macro_eval.Replace(i, strdup(""));
			}
			break;
		}
		case OTHER:
			RepeatString(out, line);
			break;
		}
		
		delete line;
	}

	return 0;
}


void Dependencies::RepeatString(ostream& out, char* str, char* check)
{
	char **text;
	Variable** Token;
	
	int n_max = 64;
	int n = 0;
	
	text = new (char *[n_max+1]);
	Token = new (Variable *[n_max]);
	int offset;
	
	
	while(1)
	{
		Token[n] = Repeater(str, 'i', offset);
		
		if(!Token[n])
			break;
		
		str[offset] = 0;	
		text[n] = str;
		
		str += offset + Token[n]->len + 3;
		
		n++;
	}
	
	int n_ck = 0;
	Variable** Token_ck = new (Variable *[n_max]);
	if( check )
	{
		while( 1 )
		{
			Token_ck[n_ck] = Repeater(check, 'i', offset);
			
			if(!Token_ck[n_ck])
				break;
			
			check[offset] = 0;
			if( !Blank(check) )
			{
				n_ck = 0;
				break;
			}
				
			
			check += offset + Token_ck[n_ck]->len + 3;
			n_ck++;
		}
		if( !Blank(check) )
			n_ck = 0;

	}
	
	
	if(n == 0)
	{
		out << str << "\n";
		return;
	}
	
	text[n] = str;
	
	int dim = (this->*Token[0]->var).Dimension();
	int i, j;
	
	
	for(j=1; j < n; j++)
	{
		if(dim != (this->*Token[j]->var).Dimension())
		{
			cerr << "Error in Template File (line "<<T_count<<")\n";
			cerr << "Incompatible loop variables: " <<
			Token[0]->name << " and " << Token[j]->name << "\n";
			cerr << "Have you included .INCLUDE_DEPENDENCIES " <<
						"in your template file?\n";
			exit(-1);
		}
	}
	
	for(i=0; i < dim; i++)
	{
		if( n_ck )
		{
			for( j=0; j<n_ck; j++)
			{
				if( !Blank( (this->*Token_ck[j]->var)[i] ) )
					break;
			}
			
			if( j == n_ck )
				continue;
		}
			
		for(j=0; j < n; j++)
			out << text[j] << (this->*Token[j]->var)[i];
			
		out << text[n] << "\n";
	}
}


Type Dependencies::LineType(char *line)
{
	char token[max_string];
	char e_c;
	int offset;
	
	istringstream tkstr(line);
	
	GetToken(tkstr, "=:\n#\\", token, e_c);
	
	if(line[0] == '.')
	{
		if(!strcmp(token,".INCLUDE") && (!e_c || isspace(e_c)))
			return INPUT;
		else if(!strcmp(token,".MACROS") && (!e_c || isspace(e_c)))
			return MACRO_LIST;
		else if(!strcmp(token, ".INCLUDE_DEPENDENCIES") && \
							(!e_c || isspace(e_c)))
			return INCLUDE_DEPS;
	}
		
	if(e_c == '=')
		return MACRO;
		
	if(e_c == ':' && line[0] != '\t')
	{
		if(Repeater(token, 'i', offset))
			return REPEAT_RULE;
		else
			return RULE;
	}
	
	return OTHER;
}

Variable* Dependencies::Repeater(char *line, char mark, int& offset)
{
	int i;
	int off0 = 0;
	Variable* key;
	
	while( (i = FindRepeat(line, mark)) > 0)
	{
		if(key = FindVariable(line, i))
		{
			offset = off0 + i - key->len;
			return key;
		}
		line += i+3;
		off0 += i+3;
	}
	
	offset = -1;
	return 0;
}
	

char* Dependencies::GetExpandLine()
{
	char *line;
	
	T_count += ::GetLine(Template, line);
	
	if(!line)
		return 0;
	
	if( FindRepeat(line, "*$") < 0)
		return line;
	
	istringstream in(line);
	char word[max_string];
	char ec;
	ostringstream out;
	while( copywhite(in,out) && GetString(in, " \t=:#\\\n", word, ec) >= 0 )
	{
		char *Xword = ExpandWord(strdup(word));
		out << Xword;
		out.put(ec);
		
		if( ec == '\\' )
		{
			char ch;
			if( in.get(ch) )
				out.put(ch);
		}
		delete Xword;
	}
	
	delete line;
    string str = out.str();
	char *oline = str.empty() ? NULL : strdup(str.c_str());
	
	if( !oline || *oline == 0 )
	{
		if( Template.eof() )
		{
			delete oline;
			return 0;
		}
	}
	
	return oline;
}

char* Dependencies::ExpandWord(char* word)
{
	int i = FindRepeat(word, "*$");
	Variable *key;
	
	if( i == -1 || !(key = FindVariable(word, i)) )
		return word;
	
	word[i- (key->len)] = '\0';
	char *post = word+i+3;
	List &var = this->*(key->var);

	ostringstream out;
	if(word[i+1] == '$')
	{
		out << word << var.Dimension() << post ;
	}
	else
	{
		for( i = 0; i< var; i++ )
		{
			if( i )
				out << " ";
			out << word << var[i] << post ;
		}
	}
	delete word;
	
	return ExpandWord(strdup(out.str().c_str())); // fix memory leak
}

Variable *Dependencies::FindVariable(char *line, int offset)
{
	Variable **key;
	
	if( !line || offset < 0 )
		return 0;
	
	for( key=KeyWord; *key; key++)
	{
		int len = (*key)->len;
		
		if( len > offset )
			continue;

		char *name = (*key)->name;
		if( !strncmp(name, line + offset - len, len) )
		{
			if( offset-len-2 >= 0  && line[offset-len-2] == '\\' )
				return *key;
				
			if(len != offset)
			{
				char c = line[offset - len -1];
				if( isalpha(c) || c=='_' )
					continue;
			}
			

			return *key;
		}
	}
	
	return 0;
}




