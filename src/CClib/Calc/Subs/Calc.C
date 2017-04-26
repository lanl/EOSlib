#include <cstring>
#include <cmath>
#include "Calc.h"


#define token input.current

Calc::Calc(int nd, int na, int ndbl, double *def_dbl)
           : var(nd), array(na), new_dbl(ndbl)
{
    default_dbl = new_dbl ? def_dbl : NULL;
    new_str = 0;
    default_str = NULL;
    InitFunc();
}

int Calc::parse(const char *line, double &value)
{
	input.Init(line);
	inbuf = 0;
	eval = 1;
	
	if( NextToken() )
	{
		// check for simple number
		if( token.IsNumber() && input.IsEOF() )
		{
			value = token.Value();
			return 0;
		}
	
		inbuf = 1;
		if( !ALexpr(value) && token.IsEof() )
			return 0;
	}
	
	value = NaN;
	return 1;
}

int Calc::ALexpr(double &value)
{
	if( Lexpr(value) )
	    return 1;

	if( !NextToken() || !token.IsOperator("?") )
	    return 0;
	inbuf = 0;

	int eval_t = 0;
	int eval_f = 0;
	if( eval )
	{
	    if( value )
		eval_t = 1;
	    else
		eval_f = 1;

	}

	int eval_0 = eval;
	eval = eval_t;
	if( Aexpr(value) )
	    return 1;
	if( !NextToken() || !token.IsOperator(":") )
	    return 1;
	inbuf = 0;

	eval = eval_f;
	if( Aexpr(value ) )
	    return 1;

	eval = eval_0;
	return 0;		
}

int Calc::Lexpr(double &value)
{
	if( NextToken() && token.IsBoolean("!") )
	{
	    inbuf = 0;
	    if( Lprimary(value) )
	        return 1;
	    if( eval )
		value = value ? 0 : 1;
	}
	else if( Lprimary(value) )
	    return 1;

	int eval_0 = eval;
	while( NextToken() && (token.IsBoolean("||") || token.IsBoolean("&&")) )
	{
	    int IsOr = token.IsBoolean("||");
	    inbuf = 0;

	    if( eval_0 )
	    {
	        if( IsOr )
		    eval = (value==0);
		else
		    eval = (value!=0);
	    }

	    if( Lprimary(value) )
		return 1;
	}
	eval = eval_0;
	return 0;	
}

int Calc::Lprimary(double &value)
{

	if( Aexpr(value) )
	    return 1;

	if( NextToken() && token.IsLogical() )
	{
	   char *op = strdup(token.String());
	   inbuf = 0;
	   double expr;

	   if( Aexpr(expr) )
	       return 1;

	   if( eval )
           {
	       if( !strcmp(op,"<") )
	           value = (value < expr);
	       else if( !strcmp(op,">") )
	           value = (value > expr);
	       else if( !strcmp(op,"==") )
	           value = (value == expr);
	       else if( !strcmp(op,"!=") )
	           value = (value != expr);
	       else if( !strcmp(op,"<=") )
	           value = (value <= expr);
	       else if( !strcmp(op,">=") )
	           value = (value >= expr);
	   }
	   delete [] op;
	}
	return 0;	
}

int Calc::Aexpr(double &value)
{
	if( term(value) )
	    return 1;
		
	while( NextToken() && (token.IsOperator("+") || token.IsOperator("-")) )
	{
	    char c = *token.String();
	    inbuf = 0;

	    double t;
	    if( term(t) )
	    	return 1;
	    if( eval )
	    {
	        if( c == '+' )
	    	    value += t;
	        else
	    	    value -= t;
	    }
	}
	return 0;	
}

int Calc::term(double &value)
{
	if( factor(value) )
	    return 1;
	
	while( NextToken() )
	{
	    if( !(token.IsOperator("*") || token.IsOperator("/")) )
	    	return 0;
	    	
	    char c = *token.String();
	    inbuf = 0;
	    double f;
	    if( factor(f) )
	    	return 1;
	    if( eval )
	    {
	        if( c == '*' )
	    	    value *= f;
	        else
	    	    value /= f;
	    }
	}
	return 0;
}


int Calc::factor(double &value)
{
	double sign = 1;
	
	while( NextToken() )
	{
	    if( !(token.IsOperator("+") || token.IsOperator("-")) )
		break;
	    if( token.IsOperator("-") )
	        sign *= -1;
	     inbuf = 0;
	}
	
	if( primary(value) )
		return 1;

	if( !NextToken() || !token.IsOperator("^") )
	{
		if( eval )
		    value *= sign;
		return 0;
	}
	
	inbuf = 0;
	double p;
	if( factor(p) )
		return 1;
	
	if( eval )
	    value = sign * pow(value,p);	
	return 0;
}

int Calc::primary(double &value)
{
	if( !NextToken() )
		return 1;
	
	if( token.IsNumber() )
	{
	    if( eval )
		value = token.Value();
	}
	else if( token.IsVariable() )
	{
        char *name = strdup(token.String());
        int status = 0;
        inbuf = 0;
		if( NextToken() && token.IsEqual() )
		{
            double RHS;
            char *eqop = strdup(token.String());
			inbuf = 0;
            CalcVar::TYPE type = var.type(name);
            if( type == CalcVar::STRING )
            {
                if( *eqop != '=' )
                    return 1;
                if( !NextToken() || !token.IsLiteral() )
                    return 1;
                if( eval )
                {
                    status = var.Last()->set(token.String());
                    value = 0.0;
                }
                inbuf = 0;
    			if( NextToken() )
    				return 1;
            }
            else
            {
    			if( Aexpr(RHS) )
    				return 1;
    			if( NextToken() && !token.IsOperator(")") )
    				return 1;
                if( eval )
                {
                    if( !strcmp(eqop,"=") )
                    {
                        value = RHS;
                        status = var.set(name,value,new_dbl);
                    }
                    else if( (status=var.get(name,value,default_dbl)) == 0 )
                    {
                        if( !strcmp(eqop, "+=") )
                            value += RHS;
                        else if( !strcmp(eqop, "-=") )
                            value -= RHS;
                        else if( !strcmp(eqop, "*=") )
                            value *= RHS;
                        else if( !strcmp(eqop, "/=") )
                            value /= RHS;
                        status = var.Last()->set(value);
                    }
                    CalcVar *v = var.Last();
                    if( v == NULL )
                        return 1;
                    v->get(value);
                }
            }
            delete [] eqop;
		}
        else if( eval )
            status = var.get(name,value,default_dbl);
        delete [] name;
		return status;
	}
	else if( token.IsFunction() )
	{
		Function *func = ValidFunction( token.String() );
		if( func )
		{
			inbuf = 0;
			double x;
			if( Aexpr(x) || !NextToken() || !token.IsOperator(")") )
				return 1;
			if( eval )
			    value = func->func(x);
		}
		else
			return SetArray(token.String(), value);
	}
	else if( token.IsOperator("(") )
	{
		inbuf = 0;
		if( ALexpr(value) || !NextToken() || !token.IsOperator(")") )
			return 1;			
	}
	else
		return 1;

	inbuf = 0;
	return 0;	
}

int Calc::SetArray(const char *name, double &value)
{
    CalcArray *A = NULL;
    if( eval && (A=array.Fetch(name)) == NULL )    
            return 1;
    
	inbuf = 0;
    int d, i1, i2;
	if( !NextToken() || !token.IsNumber() )
		return 1;
    d  = 1;
	i1 = (int) token.Value();
	inbuf = 0;
    if( !NextToken() )
        return 1;
	if( token.IsOperator(",") )
    {
		inbuf = 0;
        if( !NextToken() || !token.IsNumber() )
			return 1;
		i2 = (int) token.Value();
        d = 2;
        inbuf = 0;
        if( !NextToken() )
            return 1;
    }
	if( !token.IsOperator(")") )
		return 1;	
	inbuf = 0;
    int status = 0;
    
	if( NextToken() && token.IsOperator("=") )
	{
        CalcArrayItr *Itr = NULL;
        if( eval )
        {
            Itr = (d==1) ?  A->Iterator(i1) : A->Iterator(i1,i2);
            if( Itr == NULL )
                return 1;
        }
        int n = 0;
	    do
	    {
		    inbuf = 0;
		    if( Aexpr(value) )
            {
			    status = 1;
                break;
            }
            if( eval )
            {
                if( n>0 )
                   Itr->next();
                n++;
                if( Itr->set(value) )
                {
                    status = 1;
                    break;
                }
            }
	    } while( NextToken() && token.IsOperator(",") );
        delete Itr;
	    if(  NextToken() && !token.IsOperator(")") )
	        status = 1;
	}
    else if( eval )
	    status = (d==1) ? A->get(value,i1) : A->get(value,i1,i2);

	return status;
}



int Calc::FetchDynamicArray(const char *name, double *&ptr)
{
    ptr = NULL;
    CalcArray *a = array.Fetch(name);
    if( a == NULL )
        return -1;
    CalcArrayDynamic *da = dynamic_cast<CalcArrayDynamic*>(a);
    if( da == NULL )
        return 0;
    return da->array(ptr);
}
