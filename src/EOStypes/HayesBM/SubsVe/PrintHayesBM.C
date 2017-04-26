#include <EOS.h>
#include "HayesBM.h"

void HayesBM::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    HayesBMparams::PrintParams(out);
}

