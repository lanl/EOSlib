#include "Table.h"

Table::~Table()
{
    // Null
}

Table::Table(const char *table_type) : EOSbase("Table",table_type)
{
    if( !init )
        Init();
}

Table::Table(const Table &tbl) : EOSbase(tbl)
{
    // Null
}

int Table::Copy(const Table& tbl, int check)
{
    if( EOSbase::Copy(tbl,check) )
        return 1;
    return 0;
}

#define FUNC "Table::InitBase", __FILE__, __LINE__, this
#define calcvar(var) calc.Variable(#var,&var)
int Table::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    EOSstatus = EOSlib::abort;
    if( InitParams(p, calc, db) )
    {
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }
    EOSstatus = EOSlib::good;
    return 0;    
}

void  Table::PrintParams(ostream &out)
{
    // derived class overloads function
}

int Table::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    return 0;
}
