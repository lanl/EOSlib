#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ?  NULL : strdup(str);
}
#include <DataBase.h>

// units = "utype::uname"
// example: hydro::MKS

Convert::Convert(Units &From, Units &To) : from(NULL), to(NULL), status(0)
{
    if(  From.utype == NULL || From.utype[0] == '\0' || From.utype[0] == '*'
        || To.utype == NULL ||   To.utype[0] == '\0' ||   To.utype[0] == '*'
        || strcmp(From.utype,To.utype) || From.values==NULL || To.values==NULL )
    { // incompatible units
		status = 1;
		return;
    }
	from = From.Duplicate();
	to   = To.Duplicate();

	Parameters &values_f = *from->values;
	Parameters &values_t = *to->values;

	Calc calc_f;
	Calc calc_t;
	
	LoadFundamentalUnits(calc_f, values_f);
	LoadFundamentalUnits(calc_t, values_t);
	
	
	ParamIterator iter(values_f);
	
	const char **u;
	const char* param;
	const char* value;
	
	double v_f, v_t;
	double ratio;
	
	while( iter.Next(param,value) )
	{
		const char *statement;
		if( IsFundamental(param) )
			statement = param;
		else
		{
			Statement(param,value);
			statement = line[0];
		}
		if( calc_f.parse(statement,v_f) || calc_t.parse(statement,v_t) 
            || calc.Variable(param, v_f/v_t) )
		{
			status = 1;
			return;
		}
	}
}

Convert::~Convert()
{
	deleteUnits(from);
	deleteUnits(to);
}


double Convert::factor(const char *quantity)
{
	double value;
	if( calc.parse(quantity,value) )
		return NaN;
	return value;
}

void Convert::Statement(const char *param, const char *value)
{
// line[0] = "param=value"
	line.Reset();
	for( ; *param != '\0'; param++)
		line.Add(*param);
	line.Add('=');
	for( ; *value != '\0'; value++)
		line.Add(*value);
	line.Add('\0');	
}

void Convert::LoadFundamentalUnits(Calc &cal, Parameters &values)
{
	ParamIterator iter(values);
	const char* param;
	const char* value;

	while( iter.Next(param, value) )
	{
		const char **u;
		for(u=Units::fundamental; *u != NULL; u++)
		{
			if( !strcmp(*u,param) )
			{
				double ans;
				Statement(param,value);
				cal.parse(line[0],ans);
				break;
			}
		}
	}
}

