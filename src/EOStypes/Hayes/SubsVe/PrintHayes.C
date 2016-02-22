#include <EOS.h>
#include "Hayes.h"

void Hayes::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    HayesParams::PrintParams(out);
}
