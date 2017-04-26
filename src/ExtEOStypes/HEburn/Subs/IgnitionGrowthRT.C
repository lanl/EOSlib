#include <iostream>
#include <iomanip>
using namespace std;

#include "PTequilibrium.h"
#include "IgnitionGrowthRT.h"
#include "HotSpotHE.h"
//
//
IgnitionGrowthRT::IgnitionGrowthRT()
{
    delete [] type;
    type = Strdup("IgnitionGrowthRT");       
}

IgnitionGrowthRT::~IgnitionGrowthRT()
{
    // Null
}

HErate *IgnitionGrowthRT::Copy(PTequilibrium *eos)
{

    IgnitionGrowthRT *rate = new IgnitionGrowthRT;
    IgnitionGrowth::operator=(*this); 
    delete [] type;
    type = Strdup("IgnitionGrowthRT");
    return rate;
}

IDOF *IgnitionGrowthRT::Idof()
{
    return new HotSpotHE;
}

void IgnitionGrowthRT::PreInit(Calc &calc)
{
    IgnitionGrowth::PreInit(calc);
}

int IgnitionGrowthRT::PostInit(Calc &calc, DataBase *db)
{
    return IgnitionGrowth::PostInit(calc, db);
}

void IgnitionGrowthRT::PrintParams(ostream &out)
{
    IgnitionGrowth::PrintParams(out);
}

int IgnitionGrowthRT::ConvertParams(Convert &convert)
{
    return IgnitionGrowth::ConvertParams(convert);    
}
