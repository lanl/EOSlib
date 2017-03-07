#ifndef CCLIB_ONE_D_FUNCTION_
#define CCLIB_ONE_D_FUNCTION_

#include <cmath>

const int HUGE_INT = 0x7fffffff;	// move to MachDependent.h
const double MACH_EPS = 1.0e-14;	// move to MachDependent.h

enum OneD_status
{
	OK=0, ERROR=1,
	ZERO_LOWER_LIMIT = 2,
	ZERO_UPPER_LIMIT = 4,
	ZERO_NO_BRACKET = 6,
	ZERO_INSIDE = 8,
	ZERO_NOT_CONVERGED = 10
};

const char* zero_status(OneD_status status);

// Abstract base class OneDFunction
//	derived class must supply
//	double f(double x);
//
class OneDFunction		// Base class
{
private:
	OneD_status status;
	double y_goal;
	
	OneDFunction(const OneDFunction &);		// Not allowed
	OneDFunction& operator=(const OneDFunction&);	// Not allowed
protected:
	double PRE_ZERO(double x_guess);	// Calculate f() at end points
	double PRE_CONDITION(double x_guess);	// check for bracket
	void ZERO();				// refine bracket

	 OneDFunction();
	~OneDFunction() {}
public:
	double x0, x1;		// last bracket, y = f(x)
	double y0, y1;		// last bracket, y0 <= y1
	
	double rel_tolerance;	// default: 1.e-8
	double abs_tolerance;	// default: 1.e-14
	int max_cycles;		// maximum iterations per call, default: 64
	int n_cycles;		// iterations remaining
	
//	function f() MUST be defined in derived class
//	If argument out of range f() should call SetError() to set error status
	virtual double f(double) = 0;
	
//	inverse() returns xi such that f(xi) = y_g to within tolerance
//		x_0 and x_1 must form bracket
//		initial guess is optional
	double inverse(double y_g,double x_0, double x_1, double x_guess=HUGE)
	{
		y_goal = y_g;
		x0 = x_0;
		x1 = x_1;
		
		return PRE_ZERO(x_guess);
	}

//	option of supplying y_0 = f(x_0) and y_1 = f(x_1)
	double inverse(double y_g, double x_0, double y_0, double x_1,
					double y_1, double x_guess = HUGE)
	{
		y_goal = y_g;
		x0 = x_0;
		y0 = y_0;
		x1 = x_1;
		y1 = y_1;
		
		return PRE_CONDITION(x_guess);
	}

//	zero() returns xi such that f(xi) = 0 to within tolerance
//		x_0 and x_1 must form bracket
//		initial guess is optional
	double zero(double x_0, double x_1, double x_guess = HUGE)
	{
		y_goal = 0.0;
		x0 = x_0;
		x1 = x_1;
		
		return PRE_ZERO(x_guess);
	}

//	option of supplying y_0 = f(x_0) and y_1 = f(x_1)
	double zero(double x_0, double y_0, double x_1, double y_1,
							double x_guess = HUGE)
	{
		y_goal = 0.0;
		x0 = x_0;
		y0 = y_0;
		x1 = x_1;
		y1 = y_1;
		
		return PRE_CONDITION(x_guess);
	}
	
//	Refine bracket
//	Must first have called either zero or inverse
	double refine()
	{
		ZERO();
		return 0.5*(x0+x1);
	}

	void SetError(OneD_status stat = ERROR) { status = stat; }
	OneD_status Status() const { return status; }
	const char *ErrorStatus() const { return zero_status(status); }
	int Iterations() const { return max_cycles - n_cycles; }
};


// Abstract base class OneDFunctionI
//	derived class must supply
//	double f(int x);
//
class OneDFunctionI		// Base class
{
private:
	OneD_status status;	
	double y_goal;
	
	OneDFunctionI(const OneDFunctionI &);		// Not allowed
	OneDFunctionI& operator=(const OneDFunctionI&);	// Not allowed
protected:
	void PRE_ZERO(int x_guess);		// Calculate f() at end points
	void PRE_CONDITION(int x_guess);	// check for bracket
	void ZERO();				// refine bracket
	
	 OneDFunctionI();
	~OneDFunctionI() {}
public:
	int x0, x1;		// last bracket, y = f(x)
	double y0, y1;		// last bracket, y0 <= y1
	
	int cycles;		// number of iterations for last call
	
//	function f() MUST be defined in derived class
//	If argument out of range f() should call SetError() to set error status
	virtual double f(int) = 0;
	
//	inverse() refines bracket until |x0-x1| <=1 and f(x0) <= y_g <= f(x1)
//		x_0 and x_1 must form bracket
//		initial guess is optional
	void inverse(double y_g, int x_0, int x_1, int x_guess=HUGE_INT)
	{
		y_goal = y_g;
		x0 = x_0;
		x1 = x_1;
		
		PRE_ZERO(x_guess);
	}

//	option of supplying y_0 = f(x_0) and y_1 = f(x_1)
	void inverse(double y_g, int x_0, double y_0, int x_1, double y_1,
							int x_guess = HUGE_INT)
	{
		y_goal = y_g;
		x0 = x_0;
		y0 = y_0;
		x1 = x_1;
		y1 = y_1;
		
		PRE_CONDITION(x_guess);
	}

//	zero() refines bracket until |x0-x1| <=1 and f(x0) <= 0 <= f(x1)
//		x_0 and x_1 must form bracket
//		initial guess is optional
	void zero(int x_0, int x_1, int x_guess = HUGE_INT)
	{
		y_goal = 0.0;
		x0 = x_0;
		x1 = x_1;
		
		PRE_ZERO(x_guess);
	}

//	option of supplying y_0 = f(x_0) and y_1 = f(x_1)
	void zero(int x_0, double y_0, int x_1, double y_1,
							int x_guess = HUGE_INT)
	{
		y_goal = 0.0;
		x0 = x_0;
		y0 = y_0;
		x1 = x_1;
		y1 = y_1;
		
		PRE_CONDITION(x_guess);
	}
	
	void SetError() { status = ERROR; }
	OneD_status Status() const { return status; }
	const char *ErrorStatus() const { return zero_status(status); }
	int Iterations() const { return cycles; }
};



class HermiteInterpolate
{
	double a_0;
	double a_1;
	double a_2;
	double a_3;
	
	double x_0;
	double h;
	enum {FAILED, BELOW, INSIDE, ABOVE};
	
	

public:
	HermiteInterpolate(){a_0 = a_1 = a_2 = a_3 = x_0 = h = 0;}
	HermiteInterpolate(double y_0, double del_y, double y_0_prime,
				double y_1_prime, double x0, double del_x)
	{
		Init(y_0, del_y, y_0_prime, y_1_prime, x0, del_x);
	}
				
				
	void Init(double y_0, double del_y, double y_0_prime,
				double y_1_prime, double x0, double del_x);
	int Eval(double x, double &y);
	double RelEval(double p)
	{
		return ((a_3*p + a_2)*p + a_1)*p + a_0;
	}
};


class HermiteInterpolateN
{
	int n_dim;
	
	double *a_0;
	double *a_1;
	double *a_2;
	double *a_3;
	
	double x_0;
	double h;
	enum {FAILED, BELOW, INSIDE, ABOVE};
	
	int init;
	
	void Allocate(int n);
	void rel_val(double p, double *y, double *yp=0);
	

public:
	HermiteInterpolateN() {n_dim = 0; init=0;}
	HermiteInterpolateN(int n) {n_dim=0; init=0; Allocate(n);}
	HermiteInterpolateN(int n, double *y_0, double *del_y,
		double *y_0_prime, double *y_1_prime, double x0, double del_x)
	{
		n_dim = 0;
		Allocate(n);
		Init(y_0, del_y, y_0_prime, y_1_prime, x0, del_x);
	}
	
	void Init(double *y_0, double *del_y, double *y_0_prime,
				double *y_1_prime, double x_0, double del_x);
				
	int Eval(double x, double *y, double *yp=0);
	int RelEval(double p, double *y, double *yp=0)
	{
		if(!init)
			return FAILED;
		else
		{
			rel_val(p, y, yp);
			return 0;
		}
	}
};



#endif /* CCLIB_ONE_D_FUNCTION_ */
