#include <LocalIo.h>
#include "EOS.h"
#include <iostream>
//#include <stdlib.h>
#include <string>

using namespace std;
int main(int, char **argv)
{
	std::string file_;
        file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
	file_ += "/test_data/EOSTest.data";
        const char * file = file_.c_str();
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

    // load EOS_info()
    // but generates error since EOS:EOS::NULL does not exist
	EOS *eos = FetchEOS("EOS","NULL",db);
    if( eos )
        cerr << Error("fetch base class suceeded") << Exit;

    const char *info = db.BaseInfo("EOS");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;

    cout << info << "\n";

    cout << "sizeof(EOSbase) = " << sizeof(EOSbase) << "\n";
    cout << "sizeof(EOS) = " << sizeof(EOS) << "\n";
    return 0;
}
