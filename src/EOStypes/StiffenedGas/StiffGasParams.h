#ifndef EOSLIB_STIFF_GAS_PARAMS_H
#define EOSLIB_STIFF_GAS_PARAMS_H

class StiffGasParams
{
// P = G*(e-e_p)/V - (G+1)*P_p
// T = (e-e_p-P_p*V)/Cv
public:
    double G;    // Gruneisen coefficient
    double e_p, P_p;
    double Cv;

    StiffGasParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK() const;
};

#endif // EOSLIB_STIFF_GAS_PARAMS_H

