#include <iostream>
#include <iomanip>
using namespace std;

#include "HEburn.h"


HEburn::HEburn() : ExtEOS("HEburn")
{
    rate = NULL;
    HE   = NULL;
    ilambda = -1;
}

HEburn::HEburn(const HEburn &eos) : ExtEOS(eos)
{
    rate = NULL;
    HE   = NULL;
    ilambda = eos.ilambda;
    if( (eos.HE != NULL) && (eos.rate!=NULL) )
    {
        HE   = eos.HE->Duplicate();
        rate = eos.rate->Copy(eos.HE);
    }
    if( HE==NULL || rate==NULL || ilambda<0 )
        EOSstatus = EOSlib::abort;
}

HEburn::~HEburn()
{
    deleteHErate(rate);
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
}

ExtEOS *HEburn::Copy()
{
    ExtEOS *xeos = new HEburn(*this);
    return xeos;
}

EOS *HEburn::CopyEOS(const double *z)
{
    if( HE == NULL || ilambda<0 )
        return NULL;
    if( z[ilambda] <= 0.0 )
        return HE->eos1 ? HE->eos1->Duplicate() : NULL;
    if( 1. <= z[ilambda] )
        return HE->eos2 ? HE->eos2->Duplicate() : NULL;
    PTequilibrium *eos = new PTequilibrium(*HE);
    if( eos->lambda2 != z[ilambda] )
    {
        eos->lambda1 = 1.-z[ilambda];
        eos->lambda2 = z[ilambda];
        eos->Vav = -1.;
    }    
    return eos;
}

void HEburn::SetVerbose(int d)
{
    verbose = d;
    rate->verbose = d;
    //cerr << "DEBUG: HEburn::SetVerbose " << d << "\n";
}

void HEburn::PrintParams(ostream &out)
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

ostream &HEburn::PrintComponents(ostream &out)
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

int HEburn::ConvertParams(Convert &convert)
{
    if( EOS::ConvertParams(convert) )
        return 1;
    if( HE == NULL || HE->ConvertUnits(convert) )
        return 1;
    if( rate == NULL || rate->ConvertUnits(convert) )
        return 1;
    return 0;   
}

int HEburn::PreInit(Calc &calc)
{
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
    ilambda = -1;
    deleteHErate(rate);
    char *reactants = NULL;
    char *products  = NULL;
    char *HE_rate   = NULL;
    calc.Variable( "reactants", reactants );
    calc.Variable( "products", products );
    calc.Variable( "HErate", HE_rate );
    return 0;    
}

#define FUNC "HEburn::PostInit",__FILE__,__LINE__,this
int HEburn::PostInit(Calc &calc, DataBase *db)
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
    calc.Fetch("HErate",HE_rate);
    if( HE_rate == NULL )
    {
        EOSerror->Log(FUNC, "HE_rate not specified\n" );
        return 1;
    }
    //rate = FetchHErate(HE_rate,*db);
    rate = FetchNewHErate(HE_rate,*db);
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
    ilambda = z_ref->var_index("lambda");
    if( ilambda < 0 )
    {
        EOSerror->Log(FUNC, "lambda not in z_ref\n" );
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
//
int HEburn::var(const char *name, double V, double e, const double *z,
                         double &value)
{
    return ExtEOS::var(name,V,e,z,value);    
}
