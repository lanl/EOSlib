#ifndef EOSLIB_HAYES_PARAMS_H
#define EOSLIB_HAYES_PARAMS_H

class HayesParams
{
public:
    double V0, e0, P0, T0;              // reference state
    double K0;                          // isothermal bulk modulus
    double N;                           // K = K0*(V0/V)^N
    double Gamma0;                      // Gruneisen coefficient
    double Cv;                          // specific heat at constant V
    
    HayesParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK() const;
};

#endif // EOSLIB_HAYES_PARAMS_H
