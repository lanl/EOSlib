#ifndef EOSLIB_EOS_STATE_H
#define EOSLIB_EOS_STATE_H

class EOSstate
{
public:
    EOS *eos;

    double V;
    double e;
    double u;

    double P;
    double T;
    double S;

    double c;
    double us;

    EOSstate() : eos(NULL) {}
    EOSstate(EOS &mat, double VV, double ee, double Up=0, double Us=EOS::NaN);
    ~EOSstate();
};

ostream &operator<<(ostream &out, EOSstate &state);

#endif //  EOSLIB_EOS_STATE_H
