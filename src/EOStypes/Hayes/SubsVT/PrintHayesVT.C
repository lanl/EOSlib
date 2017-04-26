#include <EOS_VT.h>
#include "Hayes_VT.h"

void Hayes_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    HayesParams::PrintParams(out);
}

