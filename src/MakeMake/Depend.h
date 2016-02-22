// Dependency data structure for MakeMake

#include <fstream>
using namespace std;

#include "List.h"

class Dependencies;

typedef List Dependencies::* Dlist;

enum Type {MACRO, MACRO_LIST, INCLUDE_DEPS, INPUT, RULE, REPEAT_RULE, OTHER};


class Variable
{
public:
	char *name;
	int len;
	Dlist var;
	
	Variable(const char *n, Dlist Var);
	~Variable();
};


class Dependencies
{
//	static char*  keyword[];
//	int *keylength;
static Variable *KeyWord[];
private:
	int line_count;
	int T_count;
	int read_depend;
	int include;
	
	int error_count;
	
public:
	List hfiles;		// h files to check for dependencies
	List hfiles_dep;
	
	List cfiles;		// c files to check for dependencies
	List cfiles_dep;
	
	List Cfiles;		// c++ files to check for dependencies
	List Cfiles_dep;
	
	List ffiles;		// f files to check for dependencies
	List ffiles_dep;
	
	List execs;		// Executables
	List compile;		// 	Compiler, C, CC or F
	List exec_libs;		// Libraries exec[i] depends on
	List exec_ld;		// Libraries exec[i] must load, but that
				// 	are not included as explicit dependence
				
	List macro;		// macro names listed in dependencies
	List macro_eval;	// string macro[i] evaluates to
	
	List templ_macro;	// macro names listed in dependencies
	List templ_macro_eval;	// string macro[i] evaluates to
	
	List subs;		// subdirectories for local libraries
	List libs;		// names of local libraries
	
	List merge_libs;	// names of super libraries obtained by
				// 	merging smaller ones
	List merge_sublibs;	// names of sub libraries making up super lib
	
	List install_hfile;	// hfiles to be installed
	List install_lib;	// libraries to be installed
	List install_exec; 	// executables to be installed

private:	
	ifstream dep;
	ifstream Template;
	
	List include_dir;	// include directories to search
		
	List FoundLocal;
	List FoundInclude_dir;
	List IncludeFile;
	
	char *GetLine();
	
	void ProcessLine(char *line);
	int AddDepend(char *fname, List &dep_list);
	const char* MacroExpand(const char* str);
	
	Variable *FindVariable(char* line, int offset);
	Variable *Repeater(char *line, char mark, int& offset);
	
	void RepeatString(ostream& out, char *str, char *check=0);
	char *GetExpandLine();
	char *ExpandWord(char* word);
	Type LineType(char* line);
	void ProcessIflags(istream& iflags, char*& reformat);
	
public:
	int MaxError;
	
	Dependencies();
	~Dependencies();
	int GetDep(char *dep_file);
	
	int Add(char *str);
	int AddMacro(char *name, istream &line);
	int AddFile(char *file, int len = -1);
	
	void Include();
	
	void Error(const char *str);
	void Print(ostream& os);
	
	int SetTemplate(char* file);
	int ProcessTemplate(char*t_file, ostream &out);		
};
