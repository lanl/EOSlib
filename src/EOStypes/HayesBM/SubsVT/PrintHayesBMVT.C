#include <EOS_VT.h>
#include "HayesBM_VT.h"

void HayesBM_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    HayesBMparams::PrintParams(out);
}

