#ifndef EOSLIB_DATA_BASE_ITERATOR_H
#define EOSLIB_DATA_BASE_ITERATOR_H

#include "DataBase.h"

class BaseIterator
{
private:
    DataBase &db;
    Base *base;
    const char **names;
    int N;
    int i;
public:
    BaseIterator(DataBase &DB);
    ~BaseIterator() {delete [] names;}
    int Start(const char *b);
    int Next();
    operator Base* ();
};

class TypeIterator
{
private:
    DataBase &db;
    Base &base;
    Type *type;
    const char **names;
    int N;
    int i;
public:
    TypeIterator(DataBase &DB, Base &B);
    ~TypeIterator() {delete [] names;}
    int Start(const char *t);
    int Next();
    operator Type* ();
};

class ParameterIterator
{
private:
    DataBase &db;
    Type &type;
    Parameters *param;
    const char **names;
    int N;
    int i;
public:
    ParameterIterator(DataBase &DB, Type &T);
    ~ParameterIterator() {delete [] names;}
    int Start(const char *n);
    int Next();
    operator Parameters* ();
};

#endif // EOSLIB_DATA_BASE_ITERATOR_H

