#ifndef EOSLIB_ELASTIC_POLYG_VT_H
#define EOSLIB_ELASTIC_POLYG_VT_H

#include <PolyG.h>
#include <Elastic1D_VT.h>
class ElasticPolyG;
//
#define ElasticPolyG_VT_vers "ElasticPolyG_VTv2.0.1"
#define ElasticPolyG_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ElasticPolyG_VTlib_vers;
extern const char *ElasticPolyG_VTlib_date;
//
extern "C" { // DataBase functions
    char *EOS_VT_ElasticPolyG_Init();
    char *EOS_VT_ElasticPolyG_Info();
    void *EOS_VT_ElasticPolyG_Constructor();
}
//
// Base class extending EOS_VT for 1D elastic flow
class ElasticPolyG_VT : public Elastic1D_VT
{
private:
    void operator=(const ElasticPolyG_VT&);   // disallowed
    ElasticPolyG_VT *operator &();            // disallowed, use Duplicate()
public:
    //EOS_VT  *Elastic1D_VT::hydro;
    //Shear1D *Elastic1D_VT::shear; // shear = GV
    //double  Elastic1D_VT::eps_el_ref;
    PolyG *PG;
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    ElasticPolyG_VT(const char *ptype = "ElasticPolyG");
    ElasticPolyG_VT(ElasticPolyG_VT&);
    ElasticPolyG_VT(ElasticPolyG&);
    ElasticPolyG_VT *Duplicate();
   ~ElasticPolyG_VT();
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
inline ElasticPolyG_VT *ElasticPolyG_VT::Duplicate()
    { return static_cast<ElasticPolyG_VT*>(EOSbase::Duplicate()); }

#endif // EOSLIB_ELASTIC_POLYG_VT_H
