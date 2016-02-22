#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "Depend.h"
#include "subs.h"

Variable::Variable(const char *n, Dlist Var) : var(Var)
{
	name = strdup(n);
	len = strlen(name);
}

Variable::~Variable()
{
	delete name;
}

static Variable LIB("LIB",			&Dependencies::libs);
static Variable SUB("SUB",			&Dependencies::subs);
static Variable MERGE_LIB("MERGE_LIB",		&Dependencies::merge_libs);
static Variable MERGE_SUBLIB("MERGE_SUBLIB",	&Dependencies::merge_sublibs);
static Variable HFILE("HFILE", 			&Dependencies::hfiles);
static Variable HFILE_DEP("HFILE_DEP", 		&Dependencies::hfiles_dep);
static Variable CFILE("CFILE", 			&Dependencies::cfiles);
static Variable CFILE_DEP("CFILE_DEP", 		&Dependencies::cfiles_dep);
static Variable CCFILE("CCFILE", 		&Dependencies::Cfiles);
static Variable CCFILE_DEP("CCFILE_DEP", 	&Dependencies::Cfiles_dep);
static Variable FFILE("FFILE", 			&Dependencies::ffiles);
static Variable FFILE_DEP("FFILE_DEP", 		&Dependencies::ffiles_dep);
static Variable EXEC("EXEC", 			&Dependencies::execs);
static Variable COMPL("COMPL", 			&Dependencies::compile);
static Variable EXEC_LIB("EXEC_LIB", 		&Dependencies::exec_libs);
static Variable EXEC_LD("EXEC_LD", 		&Dependencies::exec_ld);
static Variable INSTALL_HFILE("INSTALL_HFILE",	&Dependencies::install_hfile);
static Variable INSTALL_LIB("INSTALL_LIB", 	&Dependencies::install_lib);
static Variable INSTALL_EXEC("INSTALL_EXEC", 	&Dependencies::install_exec);

Variable *Dependencies::KeyWord[] = {
		&LIB,  &SUB,
		&MERGE_LIB, &MERGE_SUBLIB,
		&HFILE, &HFILE_DEP,
		&CFILE, &CFILE_DEP,
		&CCFILE, &CCFILE_DEP,
		&FFILE, &FFILE_DEP,
		&EXEC, &COMPL, &EXEC_LIB, &EXEC_LD,
		&INSTALL_HFILE, &INSTALL_LIB, &INSTALL_EXEC,
		0 };
	

Dependencies::Dependencies()
{
	line_count = 0;
	T_count = 0;
	read_depend = 0;
	include = 0;

	error_count = 0;
	MaxError = 0;
}

Dependencies::~Dependencies()
{
}


// Debug printout
void Dependencies::Print(ostream& os)
{
	int i;
	
	os << "\nexecs:\n";
	for( i=0; i<execs; i++ )
		os << "\t" << execs[i] << "." << compile[i] << " : "
		   					<< exec_libs[i] << "\n"
			<< "\texec_ld = " << exec_ld[i] << "\n";
	
	os << "\nmacros:\n";
	for( i=0; i<macro; i++ )
		os << "\t" << macro[i] << " = " << macro_eval[i] << "\n";
	
	os << "\ninclude_dir:\n\t";
	for( i=0; i<include; i++ )
		os << include_dir[i] << " ";
	os << "\n";
	
	os << "\nlibs/subs:\n";
	for( i=0; i<libs; i++ )
		os << "\t" << libs[i] << " : " << subs[i] << "\n";
	
	os << "\nmerge_libs:\n";
	for( i=0; i<merge_libs; i++ )
		os << "\t" << merge_libs[i] << " : " << merge_sublibs[i]
			<< "\n";
	
	os << "\ninstall_hfile:\n";
	for( i=0; i<install_hfile; i++ )
		os << "\t" << install_hfile[i] << " ";

	os << "\ninstall_lib:\n";
	for( i=0; i<install_lib; i++ )
		os << "\t" << install_lib[i] << " ";

	os << "\ninstall_exec:\n";
	for( i=0; i<install_exec; i++ )
		os << "\t" << install_exec[i] << " ";
		
	os << "\n\ninclude = " << include << "\n";
	
	os << "\nhfiles:\n";
	for( i=0; i<hfiles; i++ )
		os << "\t" << hfiles[i] << ".h : " << hfiles_dep[i] << "\n";

	os << "\ncfiles:\n";
	for( i=0; i<cfiles; i++ )
		os << "\t" << cfiles[i] << ".c : " << cfiles_dep[i] << "\n";

	os << "\nCfiles:\n";
	for( i=0; i<Cfiles; i++ )
		os << "\t" << Cfiles[i] << ".C : " << Cfiles_dep[i] << "\n";

	os << "\nffiles:\n";
	for( i=0; i<ffiles; i++ )
		os << "\t" << ffiles[i] << ".f : " << ffiles_dep[i] << "\n";

	
	os << "\nline count " << line_count << "\n\n";

	
}


int Dependencies::GetDep(char *dep_file)
{
	dep.open(dep_file);
	
	if(!dep.good())
	{
		cerr << "Cannot open dependency file\n";
		return -1;
	}
	
	while(dep)
	{
		char* line = GetLine();
		
		if(line)
		{
			ProcessLine(line);
			delete line;
			if( include )
				break;
		}
	}
	
	read_depend = 1;
	return 0;
}


void Dependencies::Error(const char *mess)
{
	error_count++;
	
	if( !read_depend )
		cerr << "Syntax Error in Depend File: line " <<
						line_count << "\n\t";
						
	cerr << mess << "\n";
	
	if(error_count > MaxError)
		exit(-1);
}



void Dependencies::ProcessLine(char* line)
{
	char str[max_string];
	char end_char;
	int len;

	istringstream sline(line);
	len = GetToken(sline, ":=", str, end_char);
	
	if(end_char == '=')		// Macro Definition
	{
		AddMacro(str,sline);
	}
	else if(end_char == ':')	// Rule Definition
	{
		char suffix = Suffix(str, len);
		
		if(suffix == 'a')
		{			
			// Libs and Merge Libs
			
			char suffix2, ec2;
			int len2;
			char str2[max_string];

			len2 = GetToken(sline, "", str2, ec2);
			suffix2 = Suffix(str2, len2);
			
			if(suffix2 == 0)
			{
				SkipWhite(sline);
				sline.get();
				
				if(sline)
				   Error("More than one directory for library");
				else
				{
					subs << str2;
					libs << str;
				}
			}
			else if(suffix2 == 'a')
			{
				merge_libs << str;
				string out(str2);
				
				while(len2 = GetToken(sline, "", str2, ec2))
				{
					if(Suffix(str2, len2) != 'a')
					    Error("Merging with a non-library");
					
					out = out + " " + str2;
				}
				merge_sublibs = strdup(out.c_str());
			}
			else
				Error("Illegal Suffix");
		}
		else if(suffix == 0)
		{
			execs << str;
			if( cfiles[str] >= 0 )
				compile << "C" ;
			else if( Cfiles[str] >= 0 )
				compile << "CC" ;
			else if( ffiles[str] >= 0 )
				compile << "F" ;
			else
			{
				cerr << "\nError: no " << str
				     << " in cfiles, Cfiles or ffiles\n\n";
				compile << "";
			}
					
            char *ld = istream2string(sline);
			exec_ld = ld;
			istringstream in(ld);

			char word[max_string];
			string out;
			int count = 0;		
			while(in >> word)
			{
				if(word[0] != '-')
				{
					if(++count)
						out += " ";
					out += word;
				}
			}
			exec_libs = strdup(out.c_str());
		}
	}
	else if(end_char == 0)
	{
		char suffix2;
		char str2[max_string];
		char ec2;
		int len2;
		
		if( !strcmp(str, ".INSTALL") )
		{
			SkipWhite(sline);
			if( !sline.eof() )
				Error("Expected End of Line after .INSTALL");
			
			char *n_line;
			while( n_line = GetLine() )
			{
				istringstream Line(n_line);
				SkipWhite(Line);
				if( Line.eof() )
					break;				

				while(len2 = GetToken(Line, "", str2, ec2))
				{
					suffix2 = Suffix(str2, len2);
					
					if(suffix2 == 'a')
					{
						str2[len2-2] = '\0';
						install_lib << str2;
					}
					else if(suffix2 == 'h')
						install_hfile << str2;
					else if(suffix2 == 0)
						install_exec << str2;
					else
					    Error(
					     "Unknown file suffix for install");
				}
			}
		}
		else if( !strcmp(str, ".INCLUDE") )
		{
			SkipWhite(sline);
			if( !sline.eof() )
				Error("Expected End of Line after .INCLUDE");
			include = 1;
		}
	}
	else
		Error("Unexpected suffix in first token");
	
}

int Dependencies::Add(char *str)
{
	istringstream line(str);
	char arg[max_string];
	char ec;
	
	int len = GetToken(line, "=", arg, ec);
	
	if( ec == '=' )
		return AddMacro(arg, line);
	else if( ec == 0 )
		return AddFile(arg,len);
	else
		return -1;
}

int Dependencies::AddFile(char *file, int len)
{
	if( len <= 0 )
		len = strlen(file);
		
	char s = Suffix(file, len);
	
	if( s == 'h' )
	{
		file[len-2] = 0;
		if( hfiles[file] == -1 )
			hfiles << file;
		file[len-2] = '.';
		return 0;
	}
	else if( s == 'c' )
	{
		file[len-2] = 0;
		if( cfiles[file] == -1 )
			cfiles << file;
		file[len-2] = '.';
		return 0;
	}
	else if( s == 'C' )
	{
		file[len-2] = 0;
		if( Cfiles[file] == -1 )
			Cfiles << file;
		file[len-2] = '.';
		return 0;
	}
	else if( s == 'f' )
	{
		file[len-2] = 0;
		if( ffiles[file] == -1 )
			ffiles << file;
		file[len-2] = '.';
		return 0;
	}
	else
		return 1;
}

int Dependencies::AddMacro(char *name, istream &line)
{
	if( macro[name] == -1 )
	{
		macro << name;
		SkipWhite(line);
		macro_eval = istream2string(line);
	}
	return 0;
}

const char* Dependencies::MacroExpand(const char* str)
{
	for(int count = 100; count--; )
	{
		char name[max_string];
		
		if(str[0] != '$')
			return str;
			
		if(str[1] != '(')
		{
			if(str[3] == 0)
			{
				name[0] = str[2];
				name[1] = 0;
			}
			else
				return str;
		}
		else
		{
			int len = strlen(str);
			
			if(str[len-1] != ')')
				return str;
			
			strcpy(name, str+2);
			name[len-3] = 0;
		}
		
		int n = macro[name];
		
		if(n >= 0)
		{
			str = macro_eval[n];
		}
		else
		{
			n = templ_macro[name];
			
			if(n >= 0)
				str = templ_macro_eval[n];
			else
				return str;
		}
	}	
}

void Dependencies::Include()
{
	char file[max_string];
	int i;
	
	for(i=0; i < include_dir; i++)
	{
		const char *str;
		
		if( (str = MacroExpand(include_dir[i])) != include_dir[i])
			include_dir.Replace(i, strdup(str));
	}
		
	
	for(i=0; i < hfiles; i++)
	{
		strcpy(file, hfiles[i]);
		strcat(file,".h");
		AddDepend(file, hfiles_dep);
	}
	
	for(i=0; i < cfiles; i++)
	{
		strcpy(file, cfiles[i]);
		strcat(file,".c");
		AddDepend(file, cfiles_dep);
	}
	
	for(i=0; i < Cfiles; i++)
	{
		strcpy(file, Cfiles[i]);
		strcat(file,".C");
		AddDepend(file, Cfiles_dep);
	}
	
	for(i=0; i < ffiles; i++)
	{
		strcpy(file, ffiles[i]);
		strcat(file,".f");
		AddDepend(file, ffiles_dep);
	}
	
	return;
}

void Dependencies::ProcessIflags(istream& iflags, char*& reformat)
{
	char dir[max_string];
	string out;
	
	include_dir.Delete();
	FoundLocal.Delete();
	FoundInclude_dir.Delete();
	IncludeFile.Delete();
	hfiles_dep.Delete();
	cfiles_dep.Delete();
	Cfiles_dep.Delete();
	ffiles_dep.Delete();
	
	while( (iflags >> dir) )
	{
		char *s = dir;
		if( dir[0]=='-' )
		{
			if( dir[1]=='I' )
				s = dir+2;
			else
			{
				cerr << "IFLAGS syntax error `"
				     << dir << "'\n";
				continue;
			}
		}
        if( dir[0] != '$' )
            out += "-I";
		
		include_dir << s;
		out = out + s + " ";
	}
	reformat = out.empty() ? NULL : strdup(out.c_str());
}


int Dependencies::AddDepend(char *fname, List &dep_list)
{
	ifstream File(fname);
	struct stat Buffer;
	int count = 0;
	char name[1024];
	
	string DepList;
	int status;
	
	while( (status = FindInclude(File, name)))
	{
		if(status == -1)
		{
			cerr << "Bad Include Syntax in file \""<<
				fname << "\" at \""<< name << "\"\n";
			continue;
		}
			
		if(status == 1)
		{
			if( FoundLocal[name] > -1  ||
				(!stat(name, &Buffer) && (FoundLocal << name)) )
			{
				DepList = DepList + name + " ";
				count++;
				continue;
			}
		}
			
		int i, j;
		char path[max_string];
		
		for(i=0; i < include_dir; i++)
		{
			if( (j = FoundInclude_dir[name]) > -1 )
			{
				DepList = DepList + IncludeFile[j] + " ";
				count++;
				break;
			}
			
			strcpy(path, include_dir[i]);
			strcat(path, "/");
			strcat(path, name);
			
			if( !stat(path, &Buffer) )
			{
				FoundInclude_dir << name;
				IncludeFile << path;
				DepList = DepList + path + " ";
				count++;
				break;
			}
		}
	}
	dep_list << strdup(DepList.c_str());
			
	return count;
}

char *Dependencies::GetLine()
{
	char *line;
	line_count += ::GetLine(dep,line, 0);
	return line;	
}
