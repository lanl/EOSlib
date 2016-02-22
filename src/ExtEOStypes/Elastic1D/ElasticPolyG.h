#ifndef EOSLIB_ELASTIC_POLYG_H
#define EOSLIB_ELASTIC_POLYG_H

#include <PolyG.h>
#include <Elastic1D.h>
class ElasticPolyG_VT;
//
#define ElasticPolyG_vers "ElasticPolyGv2.0.1"
#define ElasticPolyG_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ElasticPolyGlib_vers;
extern const char *ElasticPolyGlib_date;
//
extern "C" { // DataBase functions
    char *EOS_ElasticPolyG_Init();
    char *EOS_ElasticPolyG_Info();
    void *EOS_ElasticPolyG_Constructor();
}
//
// Elastic1D specialized to Elastic1D::shear = Shear1D::PolyG
//                          -----------------------------------
class ElasticPolyG : public Elastic1D
{
private:
    void operator=(const ElasticPolyG&);      // disallowed
    ElasticPolyG *operator &();               // disallowed, use Duplicate()
public:
    //EOS     *Elastic1D::hydro;
    //Shear1D *Elastic1D::shear; // shear = GV
    //double  Elastic1D::eps_el_ref;
    PolyG *PG;
protected:
    EOS_VT *VT();
    int  InitParams(Parameters &, Calc &, DataBase *db);
    void PrintParams(ostream &out);
    int  ConvertParams(Convert &convert);    
public:
    ElasticPolyG(const char *ptype = "ElasticPolyG");
    ElasticPolyG(ElasticPolyG&);
    ElasticPolyG(ElasticPolyG_VT&);
    ElasticPolyG *Duplicate();
    ~ElasticPolyG();
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
    double     FD(double V, double e, double eps_el);
    double  Gamma(double V, double e, double eps_el);
    double     CV(double V, double e, double eps_el);
    int NotInDomain(double V, double e, double eps_el);
};

inline ElasticPolyG *ElasticPolyG::Duplicate()
{ return static_cast<ElasticPolyG*>(EOSbase::Duplicate()); }
#endif // EOSLIB_ELASTIC_POLYG_H
