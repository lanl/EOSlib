#include <iostream>
using namespace std;

#include "DataBase.h"

ostream &Units::Print(ostream &out, int rel_std) const
{
	out << "Units:" << utype << "::" << uname << "\n"
	       "{\n"
	       "\tValues = " << values->name << "\n"
	       "\t Names = " << names->name  << "\n"
	       "}\n";	
	
	if( rel_std)
		values->Print(out);
		
	if(names)
		names->Print(out);
	
	return out;
}

ostream &Units::PrintVarNames(ostream &out) const
{
	if( values == NULL )
		return out;
		
	ParamIterator iter(*values);
	const char *param;
	const char *value;
	while( iter.Next(param,value) )
	{
		out << param;
		if( !IsFundamental(param) )
			cout << "\t = " << value;		
		out << "\n";
	}
	return out;
}

