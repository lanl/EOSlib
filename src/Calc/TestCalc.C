#include <iomanip>
#include <string>
#include <cstdio>
#include "Calc.h"
using namespace std;



int main(int, char **argv)
{
	Calc calc;
    // disallow new var on LHS
        calc.new_dbl = NULL;
    // default for undefined variable on RHS
        // generates parse error for undefined variable on RHS
           // calc.default_dbl = NULL;
        // generates "answer not finite" for undefined variable on RHS
          // double def_dbl   = NaN;
          // calc.default_dbl = &def_dbl;
        // set undefined variable on RHS to 0
          double def_dbl   = 0.0;
          calc.default_dbl = &def_dbl;
	char line[128];
//  variable & inverse variable	
	double rho = 3;
	if( calc.Variable("rho", &rho) )
	{
		cerr << "Error, Variable rho\n";
		return 1;
	}
    CalcVar *rho_inv = new CVpinvdouble(&rho);
    if( calc.Variable("V", rho_inv) || rho_inv )
	{
		cerr << "Error, Variable V\n";
		return 1;
	}
//  variable        
	double abc = 1;
	if( calc.Variable("abc", &abc) )
	{
		cerr << "Error, Variable abc\n";
		return 1;
	}
    int k = 2;
	if( calc.Variable("k", &k) )
	{
		cerr << "Error, Variable k\n";
		return 1;
	}
//  string
    char *str = strdup("string");
    if( calc.Variable("str", &str) )
	{
		cerr << "Error, Variable str\n";
		return 1;
	}
//  1-D Fortran array
    double v[3] = {1,2,3};
    if( calc.Array("v", v, 3) )
	{
		cerr << "Error, Array v\n";
		return 1;
	}
//  2-D Fortan array
	double m[3][2] = { {1,4}, {2,5}, {3,6} };
	if( calc.Array("m", (double **)m, 2, 3) )
	{
		cerr << "Error, Array m\n";
		return 1;
	}
//  1-D dynamic Fortran array
    if( calc.DynamicArray("darray",2) )
	{
		cerr << "Error, DynamicArray m\n";
		return 1;
	}
    //	
	cout << "pre-initialized\n"
	     << "1/V = rho = " << rho << "; abc = " << abc << "\n"
         << "str = " << str << "\n"
	     << "array v(3) = (" << v[0] << "," << v[1] << "," << v[2] << ")\n" 
	     << "array m(2,3):\n"; 
	int i, j;
	for(i=0; i<2; i++)
	{
	   for( j=0; j<3; j++ )
	   	 cout << setw(12) << m[j][i];
	   cout << "\n";
	}
    cout << "darray(2+)\n";

	while( 1 )
	{	
		cerr << "Input: ";
		int i;
		int c;
		for( i=0; ;i++ )
		{
			c= cin.get();
			if( c==EOF )
				goto fini;
			if( c == '\n' )
			{
				line[i] = '\0';
				break;
			}
			line[i] = c;
		}
		if( line[0] == '\0' )
			return 0;
				
		double ans;
		if( calc.parse(line, ans) )
		{
			cerr << "parse failed\n";
			calc.PrintMarker(cerr);
		}
		
		else if( !finite(ans) )
			cerr << "answer not finite\n";
		else
			cout << "ans = " << ans << "\n";
	}
fini :	
	double value;
	if( calc.Fetch("rho", value) )
		cerr << "fetch of rho failed\n";
	cout << "rho = " << value << ", abc = " << abc << "\n";
	if( calc.Fetch("k", value) )
		cerr << "fetch of k failed\n";
	cout << "k = " << value << ", abc = " << abc << "\n";
    cout << "str = " << str << "\n";
	
	cout << "array v(3) = (" << v[0] << "," << v[1] << "," << v[2] << ")\n"; 
	cout << "array m(2,3):\n"; 
	for(i=0; i<2; i++)
	{
	   for( j=0; j<3; j++ )
           cout << setw(12) << m[j][i];
	   cout << "\n";
	}
    double *ptr=NULL;
    int n = calc.FetchDynamicArray("darray",ptr);
    if( n < 0 )
    {
		cerr << "FetchDynamicArray failed\n";
        return 1;
    }      
    cout << "darray(" << n << ") = (";
    if( n > 0 )
       cout << ptr[0];   
    for( i=1; i<n; i++ )
       cout << ", " << ptr[i];
    cout << ")\n";
    delete [] ptr;	
	return 0;
}
