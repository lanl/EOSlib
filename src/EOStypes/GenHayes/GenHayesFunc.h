#ifndef EOSLIB_GENHAYES_FUNC_EOS_H
#define EOSLIB_GENHAYES_FUNC_EOS_H
//
// Free energy, F(V,T) = ec(V) - T*IntSCvT(sT) + theta(V)*IntSCv(sT)
//  ec(V) = energy on cold curve (T=0)
//  scaled temperature, sT = T/theta(V)
//        theta(V) = Debye temperature
//  specific heat,  Cv(V,T) = SCv(sT)
//  Gruneisen coefficient function of only V
//        Gamma(V) = - dln[theta(V)]/dln(V)
//  e_thermal(V,T) = theta(V)*IntSCv(sT)
//                   IntSCv(sT)  = int_0^sT SCv(sT) d(sT)
//     entropy(sT) = IntSCvT(sT) = int_0^sT SCv(sT) d(sT)/sT
//
class GenHayesFunc  // abstract base class for generalized Hayes EOS
{
protected:
	GenHayesFunc() {}  
public:
    virtual double Pc(double V)        = 0; // cold curve P
    virtual double dPc(double V)       = 0; // dPc/dV
    virtual double ec(double V)        = 0; // ec = - Int^V_V0 dV Pc + e0
    virtual double theta(double V)     = 0; // "Debye temperature"
    virtual double Dln_theta(double V) = 0; // - d ln(theta)/d V = Gamma/V
    virtual double D2theta(double V)   = 0; // [d^2(theta)/d V^2] / theta
    virtual double SCv(double sT)      = 0; // Specific_heat(T/theta)
    virtual double IntSCv(double sT)   = 0; // Int^sT_0 dT SCv(T)
    virtual double InvIntSCv(double C) = 0; // Inverse of IntCv
    virtual double IntSCvT(double sT)  = 0; // Int^sT_0 dT/T SCv(T)
};

#endif // EOSLIB_GENHAYES_FUNC_EOS_H
