#include <iostream>
#include <iomanip>
using namespace std;

#include "HEburn2.h"

HEburn2::HEburn2() : ExtEOS("HEburn2")
{
    rate = NULL;
    HE   = NULL;
}

HEburn2::HEburn2(const HEburn2 &eos) : ExtEOS(eos)
{
    rate = NULL;
    HE   = NULL;
    if( (eos.HE != NULL) && (eos.rate!=NULL) )
    {
        HE   = eos.HE->Duplicate();
        rate = eos.rate->Copy(eos.HE);
    }
    if( HE==NULL || rate==NULL )
        EOSstatus = EOSlib::abort;
}

HEburn2::~HEburn2()
{
    deleteHErate2(rate);
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
}

ExtEOS *HEburn2::Copy()
{
    ExtEOS *xeos = new HEburn2(*this);
    return xeos;
}

EOS *HEburn2::CopyEOS(const double *z)
{
    if( EOSstatus !=  EOSlib::good )
        return NULL;
    double lambda, lambda2;
    if( rate->get_lambda(z, lambda, lambda2) )
        return NULL;
    if( lambda==0.0 )
        return HE->eos1->Duplicate();
    if( lambda==1.0 && lambda2==1.0 )
        return HE->eos2->Duplicate();

    double de = -rate->q(lambda,lambda2);
    PTequilibrium *eos = new PTequilibrium(*HE);
    eos->set_lambda2(lambda, de);   
    return eos;
}

void HEburn2::SetVerbose(int d)
{
    verbose = d;
    rate->verbose = d;
    //cerr << "DEBUG: HEburn2::SetVerbose " << d << "\n";
}

void HEburn2::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    FmtFlags old = out.setf(ios::right, ios::adjustfield);
    if( HE )
    {
        EOS *eos = HE->eos1;
        if( eos )
            out << "\treactants = \""  << eos->Type()
                << "::" << eos->Name() << "\"\n";
        eos = HE->eos2;
        if( eos )
            out << "\tproducts  = \""  << eos->Type()
                << "::" << eos->Name() << "\"\n";
        out << "\t  #PTtol  = " << HE->tol << "\n";
    }
    if( rate )
        out << "\t  HErate  = \""  << rate->Type()
            << "::" << rate->Name() << "\"\n";
    out.setf(old, ios::adjustfield);    
}

ostream &HEburn2::PrintComponents(ostream &out)
{
    if( HE )
    {
        EOS *eos = HE->eos1;
        if( eos )
            eos->PrintAll(out);
        eos = HE->eos2;
        if( eos )
            eos->PrintAll(out);
    }
    if( rate )
        rate->PrintAll(out);
    return out;
}

int HEburn2::ConvertParams(Convert &convert)
{
    if( EOS::ConvertParams(convert) )
        return 1;
    if( HE == NULL || HE->ConvertUnits(convert) )
        return 1;
    if( rate == NULL || rate->ConvertUnits(convert) )
        return 1;
    return 0;   
}

int HEburn2::PreInit(Calc &calc)
{
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
    deleteHErate2(rate);
    char *reactants = NULL;
    char *products  = NULL;
    char *HE_rate   = NULL;
    calc.Variable( "reactants", reactants );
    calc.Variable( "products", products );
    calc.Variable( "HErate2", HE_rate );
    return 0;    
}

#define FUNC "HEburn2::PostInit",__FILE__,__LINE__,this
int HEburn2::PostInit(Calc &calc, DataBase *db)
{
    const char *reactants;
    calc.Fetch("reactants",reactants);
    if( reactants == NULL )
    {
        EOSerror->Log(FUNC, "reactants not specified\n" );
        return 1;
    }
    const char *products;
    calc.Fetch("products",products);
    if( products == NULL )
    {
        EOSerror->Log(FUNC, "products not specified\n" );
        return 1;
    }    
    Parameters params;
    params.base = Strdup("EOS");
    params.type = Strdup("PTequilibrium");
    params.name = Strdup(Name());
    char *uname = Cat(units->Type(),"::",units->Name());
    params.units = Strdup(uname);
    char *eos1 = Cat("eos1 = \"",reactants, "\"");
    char *eos2 = Cat("eos2 = \"",products, "\"");
    params.Append(eos1);
    params.Append(eos2);
    params.Append("lambda1 = 1");
    delete [] eos1;
    delete [] eos2;
    delete [] uname;
    HE = new PTequilibrium;
    if( HE->Initialize(params, db) )
    {
        EOSerror->Log(FUNC, "HE->Initialize failed\n");
        delete HE;
        HE = NULL;
        return 1;      
    }
    HE->OneDFunc_abs_tol = HE->eos1->OneDFunc_abs_tol
                         = HE->eos2->OneDFunc_abs_tol = OneDFunc_abs_tol;
    HE->OneDFunc_rel_tol = HE->eos1->OneDFunc_rel_tol
                         = HE->eos2->OneDFunc_rel_tol = OneDFunc_rel_tol;
    HE->ODE_epsilon      = HE->eos1->ODE_epsilon
                         = HE->eos2->ODE_epsilon = ODE_epsilon;
    double tol;
    if( calc.Fetch("PTtol",tol)==0 )
        HE->tol = tol;
    if( std::isnan(V_ref) )
        V_ref = HE->V_ref;
    if( std::isnan(e_ref) )
        e_ref = HE->e_ref;  
    const char *HE_rate;
    calc.Fetch("HErate2",HE_rate);
    if( HE_rate == NULL )
    {
        EOSerror->Log(FUNC, "HE_rate not specified\n" );
        return 1;
    }
    rate = FetchNewHErate2(HE_rate,*db);
    if( rate == NULL )
    {
        EOSerror->Log(FUNC, "FetchHErate Failed\n" );
        return 1;
    }
    z_ref = rate->Idof();
    if( z_ref == NULL )
    {
        EOSerror->Log(FUNC, "z_ref is NULL\n" );
        return 1;
    }
    z_ref->SetParams(calc);
    if( rate->InitBurn(this->HE) )
    {
        EOSerror->Log(FUNC, "rate->InitBurn Failed\n" );
        return 1;
    }
/***********/
    if( units && rate->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "rate->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }
/*************/
    return 0;
}
