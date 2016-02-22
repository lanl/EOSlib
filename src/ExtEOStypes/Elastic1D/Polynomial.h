#ifndef EOSLIB_POLYNOMIAL_H
#define EOSLIB_POLYNOMIAL_H
#include <iosfwd>

class Polynomial
{  // f(x) = A[0] + x*A[1] + ... + x^{N-1}*A[N-1]
public:
    static int Nmax;       // max N if a[N} = NaN, default 8
protected:
    int N;
    double *A;
public:
    Polynomial() : N(0), A(NULL) {}
    Polynomial(double *a) : N(0),A(NULL)       { ReInit(a,0);} // a[N] = NaN
    Polynomial(double *a,int n) : N(0),A(NULL) { ReInit(a,n);} // N = n
    int ReInit(double *a, int n=0);                // if n=0 then a[N] = NaN
       // returns N or -1 if error
    ~Polynomial();
    double f(double x);                                 // f(x)
    double df(double x);                                // (d/dx) f(x)
    double d2f(double x);                               // (d/dx)^2 f(x)
    double d3f(double x);                               // (d/dx)^3 f(x)
    double intf(double x);                              // int dx f(x)
    int dim() const {return N;}
    std::ostream &print(std::ostream &out);
    double &operator[](int i) { return A[i];}
};

#endif  // EOSLIB_POLYNOMIAL_H
