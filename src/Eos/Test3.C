#include <LocalIo.h>
#include "EOS.h"
#include <iostream>
//#include <stdlib.h>
#include <string>

using namespace std;
int main(int, char **argv)
{
        std::string files_;
	files_ = getenv("EOSLIB_DATA_PATH");
	files_ += "/test_data/EosTest.data";
	const char *file = files_.c_str();
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

    // load EOS_info()
    // but generates error since EOS:EOS::NULL does not exist
	cout << "got here\n"; cout.flush();
	EOS *eos = FetchEOS("EOS","NULL",db);
    if( eos )
        cerr << Error("fetch base class suceeded") << Exit;

    const char *info = db.BaseInfo("EOS");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;

    cout << info << "\n";

    cout << "sizeof(EOSbase) = " << sizeof(EOSbase) << "\n";
    cout << "sizeof(EOS) = " << sizeof(EOS) << "\n";
    delete[] file;
    return 0;
}
