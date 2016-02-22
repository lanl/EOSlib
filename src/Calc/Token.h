// Token class used by Calc

#include <cstring>

class Token
{
/*
L        [a-zA-Z_]
C        [a-zA-Z0-9_\.]
D        [0-9]
d        [1-9]
I        {D}*            # should be {d}{D}* = nonzero integer
exp        ([eE][+\-]?{D}+)
num        ((0|{I})(\.{D}*)?|\.{D}+){exp}?
*/
public:
    enum Type
    {
        Error   =-2,
        Eof     =-1,        
        Empty   = 0,
        Operator= 1,    // += -= *= /= = + - * / ^ ( ) , ? :
        Logical = 2,    // < > <= == >=
        Boolean = 3,    // and or !
        Variable= 4,    // LC*
        Number  = 5,    // num
        Function= 6,    // LC*( 
        Literal = 7     // "..." or '...'
    };
    static const char *ops[];
    static const char *logic_ops[];
    static const char *bool_ops[];
private:
    char *string;
    double value;
    void Set(const char* word, double v);
    void Set(const char* ptr0, const char *ptr1, double v);

    Token(const Token&);                    // disallowed
    void operator=(const Token&);           // disallowed
public:
    Type type;
    
    Token() : type(Empty), string(0), value(NaN) {}
    ~Token();
    
    int IsError()    { return type == Error; }
    int IsEof()      { return type == Eof; }
    int IsEmpty()    { return type == Empty; }
    int IsOperator() { return type == Operator; }
    int IsOperator(const char *op)
                     { return type == Operator && !strcmp(op,string); }
    int IsEqual()    { return type == Operator &&
                              (string[0] == '=' || string[1] == '='); }         
    int IsLogical()  { return type == Logical; }
    int IsLogical(const char *op)
                     { return type == Logical && !strcmp(op,string);}
    int IsBoolean()  { return type == Boolean; }
    int IsBoolean(const char *op)
                     { return type == Boolean && !strcmp(op,string);}
    int IsVariable() { return type == Variable; }
    int IsNumber()   { return type == Number; }
    int IsFunction() { return type == Function; }
    int IsLiteral()  { return type == Literal; }
    
    void SetError()  { type=Error; Set(NULL,NaN); }
    void SetEof()    { type=Eof;   Set(NULL,NaN); }
    void SetEmpty()  { type=Empty; Set(NULL,NaN); }
    
    void SetOperator(const char* word) { type=Operator; Set(word,NaN); }
    void SetLogical(const char* word)  { type=Logical;  Set(word,NaN); }
    void SetBoolean(const char* word)  { type=Boolean;  Set(word,NaN); }
    void SetVariable(const char* word) { type=Variable; Set(word,NaN); }
    void SetNumber(double v)           { type=Number;   Set(NULL,v);   }
    void SetFunction(const char* word) { type=Function; Set(word,NaN); }
    void SetLiteral(const char* word)  { type=Literal; Set(word,NaN);  }

    void SetVariable(const char* ptr0, const char *ptr1)
                { type=Variable; Set(ptr0, ptr1, NaN); }
    void SetFunction(const char* ptr0, const char *ptr1)
                { type=Function; Set(ptr0, ptr1, NaN); }
    void SetLiteral(const char* ptr0, const char *ptr1)
                { type=Literal;  Set(ptr0, ptr1, NaN); }

    const char *String() const { return string; }
    double       Value() const { return value;  }
};
