#ifndef CCLIB_SPLINE_H
#define CCLIB_SPLINE_H

#include <LocalMath.h>


// cubic Spline
class Spline
{
private:
	void operator=(const Spline&);		// disallowed
	Spline(const Spline&);			    // disallowed
protected:
	int N;		// number of knots
	double *X;	// coordinate of knot
	double *F;	// value of function at knot
	double *F2;	// value of second derivative at knot
	int Init(double fp_0, double fp_1);	// Computes F2
public:
	Spline() : N(0), X(0), F(0), F2(0) {}
	int ReInitCopy(int n, const double x[], const double f[],
                          double fp_0, double fp_1);
	int ReInit( int n, double *&x, double *&f, double fp_0, double fp_1);
	// fp = derivative at end point
	//    = NaN for natural spline, zero second derivative
	Spline(int n, const double x[], const double f[], double fp_0, double fp_1)
						: N(0), X(0), F(0), F2(0)
	{
		(void) ReInitCopy(n, x, f, fp_0, fp_1);
	}
	Spline(int n, double **x, double **f, double fp_0, double fp_1)
						: N(n), X(*x), F(*f)
	{
		x = f = 0;
		F2 = new double[n];
		(void) Init(fp_0, fp_1);
	}
	virtual ~Spline();
	
	virtual int Interval(double x) const;
	double Evaluate(double x)  const;   // function
	double Evaluate1(double x) const;   // first derivative
	double Evaluate2(double x) const;   // second derivative
	int Evaluate(double x, double f[3]) const;
	
	int Nknot() const {return N;}
	
	double x(int i) const {return X[i];}
	double f(int i) const {return F[i];}
	double f2(int i) const {return F2[i];}
};

// Spline with equal intervals, fast lookup
// ToDo: more efficient to eliminate X[]
class Espline : private Spline
{
private:
	void operator=(const Espline&);		// disallowed
	Espline(const Espline&);			// disallowed
public:
    Espline() {}
    ~Espline();
	Espline(int n, double x0,double x1,const double f[],double fp_0,double fp_1)
          {(void)ReInitCopy(n, x0,x1, f, fp_0,fp_1);}
	Espline(int n, double x0, double x1, double **f, double fp_0, double fp_1)
          {(void)ReInit(n, x0,x1, *f, fp_0,fp_1);}  
	int ReInitCopy(int n, double x0, double x1, const double f[],
                          double fp_0, double fp_1);
	int ReInit( int n, double x0,double x1, double *&f,double fp_0,double fp_1);
	int Interval(double x) const; // equal intervals
	Spline::Evaluate;
	Spline::Evaluate1;
	Spline::Evaluate2;
	Spline::Nknot;
	Spline::x;
	Spline::f;
	Spline::f2;
};

// When f(x) is monotonic, inverse function x(f) is defined
class SplineWithInverse : public Spline
{
private:
	void operator=(const SplineWithInverse&);		    // disallowed
	SplineWithInverse(const SplineWithInverse&);		// disallowed
protected:
	int InverseInterval(double f) const;
public:
	double abs_tol;	// tolerance for iteration
	double rel_tol;
	int count;	// number of iteration in last call to Inverse(f)
	
	SplineWithInverse() : count(0)
	{
		abs_tol = 1e-4;
		rel_tol = 1e-4;
	}
	SplineWithInverse(int n, double x[],double f[], double fp_0,double fp_1)
					: Spline(n,x,f, fp_0, fp_1), count(0)
	{
		abs_tol = 1e-4;
		rel_tol = 1e-4;
	}
	SplineWithInverse(int n, double **x,double **f, double fp_0,double fp_1)
					: Spline(n,x,f, fp_0, fp_1), count(0)
	{
		abs_tol = 1e-4;
		rel_tol = 1e-4;
	}
	~SplineWithInverse() { }
	
	double Inverse(double f);
};

class SplineTree
{
public:
	double x0, x1;
	double f0, fm, f1;
	
	int n;			// total number of intervals
	SplineTree *parent;
	SplineTree *left;
	SplineTree *right;
	
	
	SplineTree(double X0, double X1, SplineTree *Parent=NULL)
		: x0(X0), x1(X1), parent(Parent)
	{
		left = right = NULL;
		n = 1;	
	}
	~SplineTree()
	{
		delete left;
		delete right;
	}
	
	void Knots(double *&x, double *&f);
	void Split()
	{
		double xm = 0.5*(x0 + x1);
		
		left = new SplineTree(x0,xm, this);
		right = new SplineTree(xm,x1, this);
		n = 2;
	}
};

extern const char *SplineStatus[];


class SplineFunction 	// abstract Base class
{
	enum Spline_status
	{
		OK=0, ERROR=1
	};
private:
	void operator=(const SplineFunction&);		// disallowed
	SplineFunction(const SplineFunction&);		// disallowed
protected:
	Spline_status status;
	SplineWithInverse spline;
	int ReInitCopy(int n, double x[], double f[], double fp_0, double fp_1)
	{
		return spline.ReInitCopy(n, x,f, fp_0,fp_1);
	}
	int ReInit( int n, double *&x, double *&f, double fp_0, double fp_1)
	{
		return spline.ReInit(n, x,f, fp_0,fp_1);
	}
public:
//	function f() MUST be defined in derived class
	double abs_tol;
	double rel_tol;
	SplineFunction(double abs_t = 1e-4, double rel_t = 1e-4)
		: abs_tol(abs_t), rel_tol(rel_t) {status = ERROR;}
	
	virtual double f(double) = 0;
	
	virtual int init(double x0, double x1,
	                 double fp_0 = NaN, double fp_1 = NaN);
	virtual SplineTree *Refine(SplineTree *ptr);
	 
	virtual double Evaluate(double x) const;
	virtual int Evaluate(double x, double f[3]) const;
	virtual double Inverse(double f);

	Spline_status Status() const { return status; }
	const char *ErrorStatus() const { return SplineStatus[status]; }
	
	int Nknot() const { return spline.Nknot(); }
	double x_k(int i) const {return spline.x(i);}
	double f_k(int i) const {return spline.f(i);}
	double f2_k(int i) const {return spline.f2(i);}
	
	int count() const {return spline.count;}
};

#endif // CCLIB_SPLINE_H
