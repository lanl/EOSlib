#include <iostream>
using namespace std;
#include "EOS.h"


int main(int, char **argv)
{
    cout << "EOSlib_mismatch = " << EOSlib_mismatch() << "\n";
    cout << EOS_vers << " : " << EOSlib_vers << "\n";
    cout << EOS_date << " : " << EOSlib_date << "\n";
    
    cout << "EOS::init = " << EOS::init << "\n";
    //cout << "EOS::ErrorDefault = " << EOS::ErrorDefault << "\n";
    cout << "Before init: " << EOS::NaN << "\n";
    EOS::Init();
    cout << "After init: " << EOS::NaN << "\n";

    EOS::ErrorDefault->Log("main", __FILE__, __LINE__, NULL,
                                "ErrorDefault->Log\n");
    return 0;
}
