#include <iostream>
using namespace std;
#include "EOS_VT.h"


int main(int, char **argv)
{
    cout << "EOS_VTlib_mismatch = " << EOS_VTlib_mismatch() << "\n";
    cout << EOS_VT_vers << " : " << EOS_VTlib_vers << "\n";
    cout << EOS_VT_date << " : " << EOS_VTlib_date << "\n";
    
    cout << "EOS_VT::init = " << EOS_VT::init << "\n";
    cerr << "EOS_VT::ErrorDefault = " << EOS_VT::ErrorDefault << "\n";

    if( !std::isnan(EOS_VT::NaN) )
        cout << "Before init: NaN = " << EOS_VT::NaN << "\n";
    EOS_VT::Init();
    if( !std::isnan(EOS_VT::NaN) )
        cout << "After init: NaN = " << EOS_VT::NaN << "\n";

    EOS_VT::ErrorDefault->Log("main", __FILE__, __LINE__, NULL,
                                "ErrorDefault->Log\n");
    return 0;
}
