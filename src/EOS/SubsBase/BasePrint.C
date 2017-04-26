#include "EOSbase.h"

void EOSbase::PrintParams(ostream &)
{
    return;
}

ostream &EOSbase::Print(ostream &out)
{
    out << (base ? base : "NULL") << ":"
        << (type ? type : "NULL") << "::"
        << (name ? name : "NULL");
    if( units )
        out << "; units=" << units->Type() << "::" << units->Name();    
    out << "\n{\n";
        PrintParams(out);
    out << "}\n";
    return out;
}

ostream &EOSbase::PrintComponents(ostream &out)
{
    return out;
}

