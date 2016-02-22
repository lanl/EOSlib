#ifndef EOSLIB_ELASTIC_CONSTGV_1D_VT_H
#define EOSLIB_ELASTIC_CONSTGV_1D_VT_H

#include <ConstGV.h>
#include <Elastic1D_VT.h>
class ElasticConstGV;
//
#define ElasticConstGV_VT_vers "ElasticConstGV_VTv2.0.1"
#define ElasticConstGV_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ElasticConstGV_VTlib_vers;
extern const char *ElasticConstGV_VTlib_date;
//
extern "C" { // DataBase functions
    char *EOS_VT_ElasticConstGV_Init();
    char *EOS_VT_ElasticConstGV_Info();
    void *EOS_VT_ElasticConstGV_Constructor();
}
//
// Base class extending EOS_VT for 1D elastic flow
class ElasticConstGV_VT : public Elastic1D_VT
{
private:
    void operator=(const ElasticConstGV_VT&);   // disallowed
    ElasticConstGV_VT *operator &();            // disallowed, use Duplicate()
public:
    //EOS_VT  *Elastic1D_VT::hydro;
    //Shear1D *Elastic1D_VT::shear; // shear = GV
    //double  Elastic1D_VT::eps_el_ref;
    ConstGV *GV;
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    ElasticConstGV_VT(const char *ptype = "ElasticConstGV");
    ElasticConstGV_VT(ElasticConstGV_VT&);
    ElasticConstGV_VT(ElasticConstGV&);
    ElasticConstGV_VT *Duplicate();
   ~ElasticConstGV_VT();
   ostream &PrintComponents(ostream &out);
// EOS_VT functions (V,T)
    using Elastic1D_VT::P;
    using Elastic1D_VT::e;
    using Elastic1D_VT::S;
    using Elastic1D_VT::P_V;
    using Elastic1D_VT::P_T;
    using Elastic1D_VT::CV;
    using Elastic1D_VT::F;
    using Elastic1D_VT::NotInDomain;
// Elastic1D_VT functions
    double P(double V, double T, double eps_el);
    double e(double V, double T, double eps_el);
    double S(double V, double T, double eps_el);
    double P_V(double V, double T, double eps_el);
    double P_T(double V, double T, double eps_el);
    double CV(double V, double T, double eps_el);
    double F(double V, double T, double eps_el);
    int NotInDomain(double V, double T, double eps_el);
};
inline ElasticConstGV_VT *ElasticConstGV_VT::Duplicate()
    { return static_cast<ElasticConstGV_VT*>(EOSbase::Duplicate()); }

#endif // EOSLIB_ELASTIC_CONSTGV_1D_VT_H
