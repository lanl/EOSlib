#include "Table.h"


int Table::AsciiTable(const char *file)
{
    return 1;   // derived class overloads virtual function
}

int Table::BinaryTable(const char *file)
{
    return 1;   // derived class overloads virtual function
}

int Table::TreeTable(const char *dir)
{
    return 1;   // derived class overloads virtual function
}
