#include <time.h>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

#include "MatToSesame.h"

char *MatToSesame::Date()
{
    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    int month = time.tm_mon+1;
    int year  = time.tm_year-100;   // tm_year = year - 1900
    int day   = time.tm_mday;
    //
    ostringstream os;
    os.fill('0');
    os << setw(2) << month
       << setw(2) << day
       << setw(2) << year;
    char *date = strdup(os.str().c_str());
    return date;
}
