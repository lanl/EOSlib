#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstdlib>

int main(int, char **argv)
{

    const char *file = argv[1] ? argv[1] : "EOSLIB_SHARED_LIBRARY_PATH/junk";

    std::string str(file);
    size_t n1 = str.find("/");
std::cout << "n1 " << n1 << "\n";
    if( n1==std::string::npos || n1==0 || n1==str.size()-1 )
    {
        std::cout << "failed\n";
        return 1;
    }
    std::string var = str.substr(0,n1);
std::cout << "var " << var << "\n";

        const char *value=getenv(var.c_str());
        if( value == NULL )
        {
            std::cout << "ERROR no environment variable\n";
            return 1;
        }
std::cout << "env(var) " << value << "\n";
    str.replace(0,n1,value);
std::cout << "str " << str << "\n";
}
