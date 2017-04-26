#include <OneDFunction.h>


OneDFunction::OneDFunction()
{
	status = OK;
	rel_tolerance = 1.e-8;
	abs_tolerance = 1.e-14;
	n_cycles = max_cycles = 64;
	y_goal = 0.0;
}

OneDFunctionI::OneDFunctionI()
{
	status = OK;
	cycles = 0;
}




const char* zero_status(OneD_status status)
{
	switch(status)
	{
	case OK:
		return "good";
	case ERROR:
		return "ERROR";
	case ZERO_LOWER_LIMIT:
		return "Error at lower limit";
	case ZERO_UPPER_LIMIT:
		return "Error at upper limit";
	case ZERO_NO_BRACKET:
		return "Error no bracket";
	case ZERO_INSIDE:
		return "Error function failed within bracket";
	case ZERO_NOT_CONVERGED:
		return "Error, did not converge";
	default:
		return "Illegal error status";
	}
}
