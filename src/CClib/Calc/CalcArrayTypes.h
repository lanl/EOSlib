#ifndef CCLIB_CALC_ARRAY_TYPES_H
#define CCLIB_CALC_ARRAY_TYPES_H

class CalcArrayItr;
class CalcArray         // base class, default c-convention
{
protected:
    int dim;            // dimension: 1, 2 or 0 on error
    double *ptr;
    int min1, max1;     // index 1
    int min2, max2;     // index 2
    int n1, n2;         // dimension of index 1 and 2
public:
    CalcArray() : dim(0), ptr(NULL)  {}     // needed for NameArray<CalcVar>
    CalcArray(double *p, int Mn1, int Mx1, int Mn2=0, int Mx2=-1);
    virtual ~CalcArray();                   // ptr not deleted
    
    int Dim()  const {return dim;}
    int Min1() const {return min1;}
    int Max1() const {return max1;}
    int Min2() const {return min2;}
    int N1()   const {return n1;}
    int N2()   const {return n2;}
    
    int get(double &value, int i1);
    int set(double  value, int i1);
    int get(double &value, int i1, int i2);
    int set(double  value, int i1, int i2);
    
    virtual double *index(int i1);
    virtual double *index(int i1, int i2);
    virtual void next(int &i1, int &i2);
    virtual CalcArrayItr *Iterator(int i1);
    virtual CalcArrayItr *Iterator(int i1, int i2);
//
    double &operator [](int i) {return ptr[i];}
};

class CalcArrayItr      // base class, default c-convention
{
protected:
    CalcArray *array;   // NULL if error
    int i1;             // current value of index 1
    int i2;             // current value of index 2
    double *ptr();      // ptr for get and set
public:
    CalcArrayItr(CalcArray *a, int I1);
    CalcArrayItr(CalcArray *a, int I1, int I2);
    ~CalcArrayItr();
    
    int get(double &value);
    int set(double  value);
    int next();
    operator int() { return ptr()!=NULL;}
};

// derived types

// CalcArrayF follows fortran convention
// A(i)   -> a[i-1]
// A(i,j) -> a[j-1][i-1]
class CalcArrayF : public CalcArray
{
public:
    CalcArrayF(double *p, int N) : CalcArray(p, 1,N) {}
    CalcArrayF(double *p, int Nrow, int Ncol) : CalcArray(p, 1,Nrow, 1,Ncol) {}
    ~CalcArrayF();                      // CalcArray::ptr not deleted
    
    double *index(int i1);
    double *index(int i1, int i2);
    void next(int &i1, int &i2);
    CalcArrayItr *Iterator(int i1);
};

class CalcArrayDynamic : public CalcArray   // 1-D, fortran convention
{
private:
    int Realloc();
    double undef;           // initial value for added elements
    int nmax;               // maximum number of elements used <= n1
public:
    CalcArrayDynamic(int N=32, double *p=NULL, double udef=0.0);
    ~CalcArrayDynamic();                    // deletes CalcArray::ptr
    
    double *index(int i1); // allows doubling, i1 <= 2*n1
    int array(double *&p); // return dimension and current array
                           // user must delete [] p
};

#endif // CCLIB_CALC_ARRAY_TYPES_H
