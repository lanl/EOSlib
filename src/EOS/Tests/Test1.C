#include <iostream>
using namespace std;
#include "EOS.h"
#include <LocalMath.h>


int main(int, char **argv)
{
    cout << "EOSlib_mismatch = " << EOSlib_mismatch() << "\n";
    cout << EOS_vers << " : " << EOSlib_vers << "\n";
    cout << EOS_date << " : " << EOSlib_date << "\n";
    
    cout << "EOS::init = " << EOS::init << "\n";
    cerr << "EOS::ErrorDefault = " << EOS::ErrorDefault << "\n";

    if( !std::isnan(EOS::NaN) )
        cout << "Before init: NaN = " << EOS::NaN << "\n";
    EOS::Init();
    if( !std::isnan(EOS::NaN) )
        cout << "After init: NaN = " << EOS::NaN << "\n";

    EOS::ErrorDefault->Log("main", __FILE__, __LINE__, NULL,
                                "ErrorDefault->Log\n");
    return 0;
}
