#ifndef CCLIB_CALC_H
#define CCLIB_CALC_H

#include <LocalMath.h>
#include <iosfwd>

#include "Token.h"          // class Token;
#include "Scanner.h"        // class Scanner;
#include "CalcVars.h"       // class CalcVars;
#include "CalcArrays.h"     // class CalcArrays;

/************************************************************************
     Recursive Descent Parser for Grammar

parse    = ALexpr EOF

ALexpr   = Lexpr                # Arithmetic or Logical expression
         | Lexpr ? Aexpr : Aexpr

Lexpr    = Lprimary             # Boolean expression
         | ! Lprimary           # ! higher precedence than and, or
         | Lexpr and Lprimary
         | Lexpr or  Lprimary

Lprimary = Aexpr
         | Aexpr <  Aexpr       # Logical primitives
         | Aexpr >  Aexpr
         | Aexpr <= Aexpr
         | Aexpr == Aexpr
         | Aexpr != Aexpr
         | Aexpr >= Aexpr

Aexpr    = term                 # Arithmetic expression
         | Aexpr + term         # left associative
         | Aexpr - term         # left associative

term     = factor
         | term * factor        # left associative
         | term / factor        # left associative

factor   = primary
         | primary ^ factor     # right associative    
         | + factor             # unary + lower precedence than ^
         | - factor             # unary - lower precedence than ^
                                # -2^-3^-4 = -(2^(-(3^-4)))
primary  = number
         | variable
         | variable [+-* /]= Aexpr     # right associative, high precedence
         | ( ALexpr )
         | function( Aexpr )
         | array( number )
         | array( number ) = Aexpr [, Aexpr]*
         | array( number, number )
         | array( number, number ) = Aexpr [, Aexpr]*

# array(i,j) = array[j+1][i+1] in "C"
#              Fortran convention, row is fast index
#              index >= 1, i.e. array(0,1) is parse error!

ToDo : replace with non-recursive operator precedence based algorithm
       note: x = 1 < 2, and x= 1?2:3 are parse errors
         but x = (1<2), and x=(1?2:3) are legal
       Evaluation of and, or, ? : same convention as in C

************************************************************************/

class Calc
{
private:
    class Function
    {
    public:
        char *name;
        double (*func)(double);
        Function(const char *n, double (*f)(double));
        ~Function();
    };
    static int init_func;
    static Function **Functions;
    void InitFunc();
protected:
    Scanner input;
    CalcVars var;
    CalcArrays array;
    int eval;
    
    int inbuf;        // token in buffer
    int NextToken()
    {
        if( inbuf ) return 1;
        return (inbuf = input.NextToken());
    }
    
    int ALexpr(double &value);
    int Lexpr(double &value);
    int Lprimary(double &value);
    int Aexpr(double &value);
    int term(double &value);
    int factor(double &value);
    int primary(double &value);
    
    Function *ValidFunction(const char *name);
    int SetArray(const char *name, double &value);
private:
    Calc(const Calc&);                  // disallowed
    void operator=(const Calc&);        // disallowed
public:
    int new_dbl;                // LHS
    int new_str;                // LHS
    double *default_dbl;        // RHS
    const char *default_str;    // RHS
    Calc(int nd=16, int na=4, int ndbl=1, double *def_dbl=NULL);
    ~Calc() {}
    
    int parse(const char *line, double &value);
    int Variable(const char *name, double *ptr)   {return var.AddVar(name,ptr);}
    int Variable(const char *name, double val)    {return var.AddVar(name,val);}
    int Variable(const char *name, int *ptr)      {return var.AddVar(name,ptr);}
    int Variable(const char *name, int val)       {return var.AddVar(name,val);}
    int Variable(const char *name, char **val)    {return var.AddVar(name,val);}
    int Variable(const char *name,const char *val){return var.AddVar(name,val);}
    int Variable(const char *name, CalcVar *&ptr) {return var.AddVar(name,ptr);}
    
    int Array(const char *name, double *ptr, int n)
                    { return array.AddF(name,ptr,n); }
    int Array(const char *name, double **ptr, int row, int col)
                    { return array.AddF(name,ptr,row,col); }
    int ArrayC(const char *name, double *ptr, int n)
                    { return array.AddC(name,ptr,n); }
    int ArrayC(const char *name, double **ptr, int row, int col)
                    { return array.AddC(name,ptr,row,col); }
    int Array(const char *name, CalcArray *&ptr) {return array.Add(name,ptr);}
    int DynamicArray(const char *name, int N=32,double *p=NULL, double udef=0.0)
                    { return array.AddDynamic(name,N,p,udef);}
       
    int Fetch(const char *name, double &x)        { return var.get(name,x);}
    int Fetch(const char *name, const char *&x)   { return var.get(name,x);}
    CalcVar *Fetch(const char *name)              { return var.Fetch(name);}
    CalcArray *FetchArray(const char *name)       { return array.Fetch(name);}
    int FetchDynamicArray(const char *name, double *&ptr);
        // user responsible for deleting ptr
    
    std::ostream &PrintMarker(std::ostream &out) { return input.PrintMarker(out); }
    const char *NextChar() { return input.NextChar(); }
};

#endif // CCLIB_CALC_H
