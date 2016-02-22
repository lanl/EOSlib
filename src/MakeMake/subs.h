#include <fstream>

using namespace std;


int RemoveComment(char *line);
int GetLine(istream& in, char *&line, int comment = 1);
int GetString(istream& line, const char *term, char* str, char& end_char);
int GetToken(istream& line, const char *term, char* str, char& end_char);
int SkipWhite(istream& s);
int copywhite(istream &in, ostream &out);

int Blank(char *str);

char Suffix(char *str, int len = -1);
char *istream2string(istream& s);

int FindInclude(ifstream& F, char *name);
int FindRepeat(const char *line, char m);
int FindRepeat(const char *line, const char* m);

const int max_string = 1024;

