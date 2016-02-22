#include <string>
using namespace std;
#include <Calc.h>
#include "DataBaseParameters.h"

int ParamIterator::Next(const char *&param, const char *&value)
{
    if( i >= params->Nparams )
        return 0;
    param = params->param[i];
    value = params->value[i++];
    return 1;
}

char *ParamIterator::Line()
{
    if( i >= params->Nparams )
        return NULL;
    delete[]line;
    string str(params->param[i]);
    str = str + "=" +  params->value[i];
    line = strdup(str.c_str());
    return line;
}

char *ParamIterator::Parse(Calc &calc)
{
    for( ; i < params->Nparams; ++i )
    {
        delete[]line;
        string str(params->param[i]);
        str = str + "=" +  params->value[i];
        line = strdup(str.c_str());
        double ans;
        if( calc.parse(line, ans) )
            return line;
    }
    return NULL;
}

