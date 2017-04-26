#ifndef EOSLIB_ELASTIC_CONSTGV_H
#define EOSLIB_ELASTIC_CONSTGV_H

#include <ConstGV.h>
#include <Elastic1D.h>
class ElasticConstGV_VT;
//
#define ElasticConstGV_vers "ElasticConstGVv2.0.1"
#define ElasticConstGV_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ElasticConstGVlib_vers;
extern const char *ElasticConstGVlib_date;
//
extern "C" { // DataBase functions
    char *EOS_ElasticConstGV_Init();
    char *EOS_ElasticConstGV_Info();
    void *EOS_ElasticConstGV_Constructor();
}
//
// Elastic1D specialized to Elastic1D::shear = Shear1D::ConstGV
//                          -----------------------------------
class ElasticConstGV : public Elastic1D
{
private:
    void operator=(const ElasticConstGV&);      // disallowed
    ElasticConstGV *operator &();               // disallowed, use Duplicate()
public:
    //EOS     *Elastic1D::hydro;
    //Shear1D *Elastic1D::shear; // shear = GV
    //double  Elastic1D::eps_el_ref;
    ConstGV *GV;
protected:
    EOS_VT *VT();
    int  InitParams(Parameters &, Calc &, DataBase *db);
    void PrintParams(ostream &out);
    int  ConvertParams(Convert &convert);    
public:
    ElasticConstGV(const char *ptype = "ElasticConstGV");
    ElasticConstGV(ElasticConstGV&);
    ElasticConstGV(ElasticConstGV_VT&);
    ElasticConstGV *Duplicate();
    ~ElasticConstGV();
    ostream &PrintComponents(ostream &out);
// EOS functions (V,e)
    using Elastic1D::P;
    using Elastic1D::T;
    using Elastic1D::S;
    using Elastic1D::c2;
    using Elastic1D::Gamma;
    using Elastic1D::CV;
    double    FD(double V, double e);
    using Elastic1D::NotInDomain;
// Elastic1D functions (V,e,eps_el)
    double      P(double V, double e, double eps_el);
    double Phydro(double V, double e, double eps_el);
    double Pshear(double V, double e, double eps_el);
    double   Pdev(double V, double e, double eps_el);    
    double      T(double V, double e, double eps_el);
    double      S(double V, double e, double eps_el);
    double     c2(double V, double e, double eps_el);
    double  Gamma(double V, double e, double eps_el);
    double     CV(double V, double e, double eps_el);
    double     FD(double V, double e, double eps_el);
    int NotInDomain(double V, double e, double eps_el);
};

inline ElasticConstGV *ElasticConstGV::Duplicate()
{ return static_cast<ElasticConstGV*>(EOSbase::Duplicate()); }
#endif // EOSLIB_ELASTIC_CONSTGV_H
