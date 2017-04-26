#ifndef EOSLIB_DATA_FILE_H
#define EOSLIB_DATA_FILE_H

#include <fstream>
using std::ifstream;

class Line;
class DataFile;

//
class Line
{
private:
    int dim;
    int n;
    char *line;
    
    void ReAlloc();

    Line(const Line&);                  // disallowed
    void operator=(const Line&);        // disallowed
public:
    Line(int dim = 0);
    ~Line();
    
    void Add(char c);
    char *operator[](int i);
    void Reset() { n = 0; }
};

class DataFile
{
private:
    Line line;                          // buffer for current line
    char *last_word;
    char *word;                         // current word in line
    // words separated by ';' with ' ', '\t' and comments deleted
    // '{' & '}' are words

    DataFile();                         // disallowed
    DataFile(const DataFile&);          // disallowed
    void operator=(const DataFile&);    // disallowed
public:
    const char *status;
    ifstream &in;
    int lines;                          // lines in file read

    DataFile(ifstream &IN) : last_word(NULL), word(NULL),
        status(NULL), in(IN), lines(0), chars_read(0),words(0) {}
    ~DataFile() { delete [] status;}
    int NextLine();
    int chars_read;                     // chars read in last line
    int words;                          // words left in line
    char *NextWord(int online = 1, int delete_space=0);
    char *LastWord(int delete_space=0);
    char *Peek() {return word;}
};

int IsBlank(const char *word);
char *DeleteSpace(char *str, int flag = 7);

#endif // EOSLIB_DATA_FILE_H
