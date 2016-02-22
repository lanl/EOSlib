#ifndef EOSLIB_RIEMANN_H
#define EOSLIB_RIEMANN_H

class RiemannSolver // abstract base class
{
private:    
    RiemannSolver();                // disallowed
    RiemannSolver(const RiemannSolver&);        // disallowed
    void operator=(const RiemannSolver&);       // disallowed
    RiemannSolver *operator &();            // disallowed
protected:
    EOS *eos_l;
    EOS *eos_r;
    RiemannSolver(EOS &left, EOS &right)
        : eos_l(left.Duplicate()), eos_r(right.Duplicate()) { }
public:
    virtual ~RiemannSolver();
    virtual int Solve(const HydroState &state_l, const HydroState &state_r,
                        WaveState &left, WaveState &right) = 0;
};


#endif // EOSLIB_RIEMANN_H
