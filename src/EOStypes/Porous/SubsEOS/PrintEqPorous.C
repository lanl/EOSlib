#include <iostream>
#include "Porous.h"

void EqPorous::PrintParams(ostream &out)
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

    out << "\t" << "PhiEq = \"";
    if( phi_eq )
    {
        const char *phi_type = phi_eq->Type();
        const char *phi_name = phi_eq->Name();
        out << (phi_type ? phi_type : "NULL") << "::"
            << (phi_name ? phi_name : "NULL") << "\"\n";
    }
    else
        out << "NULL\n";

    out << "#\t  phi_max = " << phi_max << "\n"
        << "#\t    B_max = " << B_max   << "\n"
        << "#\t    dBdphi_max = " << dBdphi_max   << "\n";
}

ostream &EqPorous::PrintComponents(ostream &out)
{
    if( solid )
        solid->PrintAll(out);
    if( phi_eq )
        phi_eq->Print(out);
    return out;
}

