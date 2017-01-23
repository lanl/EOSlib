#include <iostream>
using namespace std;
#include "EOS.h"


int main(int, char **argv)
{
    cerr << "EOSlib_mismatch = " << EOSlib_mismatch() << "\n";
    cerr << EOS_vers << " : " << EOSlib_vers << "\n";
    cerr << EOS_date << " : " << EOSlib_date << "\n";
    
    cerr << "EOS::init = " << EOS::init << "\n";
    cerr << "EOS::ErrorDefault = " << EOS::ErrorDefault << "\n";
    cerr << "Before init: " << EOS::NaN << "\n";
    EOS::Init();
    cerr << "After init: " << EOS::NaN << "\n";

    EOS::ErrorDefault->Log("main", __FILE__, __LINE__, NULL,
                                "ErrorDefault->Log\n");
    return 0;
}
