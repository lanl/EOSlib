#include <ErrorHandler.h>
#include "Spline.h"

class Vec
{
public:
	int N;
	double *V;
	
	Vec(int n=1024) : N(n)
	{
		V = new double[N];
	}
	~Vec()
	{
		delete [] V;
	}
	
	
	void allow(int n)
	{
	// guarantee V[n] is allocated
		if( n < N )
			return;
		int N0 = N;
		double *V0 = V;
		N *= 2;
		V = new double[N];
		for(int i=0; i<N0; i++)
			V[i] = V0[i];
		delete [] V0;	
	}
	double &operator[](int i) {return V[i];}
	operator double* () {return V;}
	double *dealloc()
	{
		double *v = V;
		V = 0;
		N = 0;
		return v;
	}
};


SplineTree *FirstNode(SplineTree *st)
{
	if( st == NULL )
		return NULL;
	
	while( st->left != NULL )
		st = st->left;
	
	return st;
}

SplineTree *NextNode(SplineTree *st)
{
	if( st == NULL )
		return NULL;
	
	SplineTree *parent;
	for( ; (parent = st->parent) != NULL; st = parent )
	{
		if( parent->left == st )
			return FirstNode(parent->right);
		parent->n = parent->left->n + parent->right->n;
	}
	return NULL;
}


void SplineTree::Knots(double *&x, double *&f)
{
	x = new double[n+1];
	f = new double[n+1];

	int i;
	SplineTree *st = FirstNode(this);
	for(i=0; st; i++, st = NextNode(st))
	{
		x[i] = st->x0;
		f[i] = st->f0;
	}

	x[i] = x1;
	f[i] = f1;
}


int SplineFunction::init(double x0, double x1, double fp_0, double fp_1)
{
	SplineTree ST(x0,x1);
	
	ST.x0 = x0;
	ST.x1 = x1;

	ST.n = 1;

	ST.f0 = f(x0);
	ST.f1 = f(x1);
	
	double xm = 0.5*(x0+x1);
	ST.fm = f(xm);
	
	
	SplineTree *ptr = &ST;
	
	for( ;ptr; ptr=NextNode(ptr))
	{
		ptr=Refine(ptr);
	}
	
	double *x, *f;
	ST.Knots(x,f);
	status = spline.ReInit(ST.n+1, x,f, fp_0,fp_1) ? ERROR : OK;
	spline.abs_tol = abs_tol;
	spline.rel_tol = rel_tol;
	
	return status;
};

const char *SplineStatus[] = {"OK","ERROR"};

SplineTree *SplineFunction::Refine(SplineTree *ptr)
{
	if( ptr == NULL )
		return NULL;
		
//	for( ; ; )
	for(int i=0 ; i<30 ; i++)
	{
		// compute quadratic polynomial
		//	f(y) = fm + a*y + 0.5*b*y^2
		//	interval (x0, x1) scaled to (-1, 1)
		
		double a = 0.5*(ptr->f1  - ptr->f0);
		double b = ptr->f1 - 2*ptr->fm + ptr->f0;
		
		
		double xl = 0.25*(ptr->x1+3*ptr->x0); // midpoint left inteval
		double xr = 0.25*(3*ptr->x1+ptr->x0); // midpoint right inteval

		double fl = f(xl);
		double fr = f(xr);
		
		double pl = ptr->fm + 0.5*(-a+0.25*b);
		double pr = ptr->fm + 0.5*(a+0.25*b);
		
		if( abs(pl-fl) < max(abs_tol, rel_tol*abs(fl))
		    && 	abs(pr-fr) < max(abs_tol, rel_tol*abs(fr)) )
		{
			return ptr;
		}
		ptr->Split();
		SplineTree *left = ptr->left;
			left->f0 = ptr->f0;
			left->fm = fl;
			left->f1 = ptr->fm;
		SplineTree *right = ptr->right;
			right->f0 = ptr->fm;
			right->fm = fr;
			right->f1 = ptr->f1;
		ptr = left;
	}
	
	LocalError->Log("SplineFunction::Refine", "failed\n");
	return NULL;	
}

	


double SplineFunction::Evaluate(double x) const
{
	return spline.Evaluate(x);
}

int SplineFunction::Evaluate(double x, double f[3]) const
{
	if( status == OK )
		return spline.Evaluate(x,f);
	else
		return status;
}

double SplineFunction::Inverse(double f)
{
	return spline.Inverse(f);
}
