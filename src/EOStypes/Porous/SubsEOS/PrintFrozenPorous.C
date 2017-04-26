#include <iostream>
#include "Porous.h"

void FrozenPorous::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    out << "\t" << "EOS   = \"";
    if( solid )
    {
        const char *solid_type = solid->Type();
        const char *solid_name = solid->Name();
        out << (solid_type ? solid_type : "NULL") << "::"
            << (solid_name ? solid_name : "NULL") << "\"\n";
    }
    else
        out << "NULL\n";
        
    out << "\t" << "phieq = \"";
    if( phi_eq )
    {
        const char *phi_eq_type = phi_eq->Type();
        const char *phi_eq_name = phi_eq->Name();
        out << (phi_eq_type ? phi_eq_type : "NULL") << "::"
            << (phi_eq_name ? phi_eq_name : "NULL") << "\"\n";
    }
    else
        out << "NULL\n";

    out << "\t  phi = " << phi_f << "\n"
        << "\t    B = " << B_f   << "\n";
}

ostream &FrozenPorous::PrintComponents(ostream &out)
{
    if( solid )
        solid->PrintAll(out);
    if( phi_eq )
        phi_eq->Print(out);
    return out;
}

