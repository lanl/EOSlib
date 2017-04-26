#ifndef EOSLIB_SPLIT_WORD_H
#define EOSLIB_SPLIT_WORD_H

#include <cstdlib>
#include <String1.h>

class SplitWord
{
private:
    const char *word;
    int n;
    int Seek(char c);
    char *Copy();

    void operator=(const SplitWord&);           // disallowed
    SplitWord(const SplitWord&);                // disallowed
public:
    char *param;
    char *value;
    
    SplitWord(const char * Word) : word(Word), param(NULL), value(NULL) {}
    ~SplitWord() { delete [] param; delete [] value;}
    int Next();        
};

#endif // EOSLIB_SPLIT_WORD_H
