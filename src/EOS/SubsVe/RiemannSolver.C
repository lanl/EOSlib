#include "EOS.h"
#include "Riemann_gen.h"
#include "Isentrope_ODE.h"

RiemannSolver::~RiemannSolver()
{
    deleteEOS(eos_l);
    deleteEOS(eos_r);
}

//

RiemannSolver_generic::RiemannSolver_generic(EOS &left, EOS &right)
        : RiemannSolver(left,right), Wave_l(NULL), Wave_r(NULL)
{
    //abs_tolerance = max(left.OneDFunc_abs_tol, right.OneDFunc_abs_tol);
    //rel_tolerance = max(left.OneDFunc_rel_tol, right.OneDFunc_rel_tol);
    abs_tolerance = left.OneDFunc_abs_tol + right.OneDFunc_abs_tol;
    rel_tolerance = left.OneDFunc_rel_tol + right.OneDFunc_rel_tol;
}

RiemannSolver_generic::~RiemannSolver_generic()
{
    delete Wave_l;
    delete Wave_r;
}

double RiemannSolver_generic::f(double p)
{
    if( Wave_l->P(p,LEFT,left_f) )
        return EOS::NaN;
    if( Wave_r->P(p,RIGHT,right_f) )
        return EOS::NaN;   
    return right_f.u - left_f.u;
}

int RiemannSolver_generic::Solve(
            const HydroState &state_l, const HydroState &state_r,
                   WaveState &left,           WaveState &right)
{
    delete Wave_l;  Wave_l = NULL;
    delete Wave_r;  Wave_r = NULL;
    
    if( eos_l->Evaluate(state_l,left_f) || eos_r->Evaluate(state_r,right_f) )
        return -1;

    P_l =  left_f.P;
    u_l =  left_f.u;
    P_r = right_f.P;
    u_r = right_f.u;
    // linearized solution
    double Z_l =  left_f.us/left_f.V;  // Z = rho*c
    double Z_r = right_f.us/right_f.V; // Z = rho*c
    u_c  = (P_l-P_r + Z_r*u_r+Z_l*u_l)/(Z_l+Z_r);
    P_c  = ((u_l-u_r)*Z_l*Z_r + Z_l*P_r+Z_r*P_l)/(Z_l+Z_r);  
    // set Wave_{l,r} and bounds (Pmin,Pmax)
    if( P_l < P_r && u_l <= u_r )
    {   // wave_r = isentrope
        int Q = QuadrantI(state_l, state_r);
        if( Q == -1 )
            return 1;       // error
        else if( Q == 1 )
        { // vacuum or degenerate
            left  = left_f;
            right = right_f;
            return 0;
        }
    }
    else if( P_l <= P_r && u_l > u_r )
    {   // wave_l = shock
        if( QuadrantII(state_l, state_r) == -1 )
            return 2;       // error
    }
    else if( P_l > P_r && u_l >= u_r )
    {   // wave_r = shock
        if( QuadrantIII(state_l, state_r) == -1 )
            return 3;       // error
    }
    else if( P_l >= P_r && u_l < u_r )
    {   // wave_l = isentrope
        int Q = QuadrantIV(state_l, state_r);
        if( Q == -1 )
            return 4;       // error
        else if( Q == 1 )
        { // vacuum or degenerate
            left = left_f;
            right = right_f;
            return 0;
        }
    }
    else // P_l == P_r and u_l == u_r
    {
        Wave_l = eos_l->shock(state_l);
        Wave_r = eos_r->shock(state_r);
        if( Wave_l==NULL || Wave_r==NULL )
            return 5;
        return (Wave_l->P(P_l,LEFT,left)||Wave_r->P(P_r,RIGHT,right)) ?  5 : 0;
    }
    
    double P = 0.5*(Pmin+Pmax);   
    if( Pmax - Pmin > max(abs_tolerance, rel_tolerance*P) )
    {
        P = zero(Pmin,Pmax);
        if( Status()== ZERO_NO_BRACKET )
        {
           Pmin *= 0.99;
           Pmax *= 1.01;
           P = zero(Pmin,Pmax);
        }
        if( Status() )
        {
            eos_l->ErrorHandler()->Log("RiemannSolver_generic::Solve",
                   __FILE__, __LINE__, eos_l,
                   "zero failed with status: %s\n", ErrorStatus());
            return -1;
        }
    }
    (void) f(P);
    
    left  = left_f;
    right = right_f;
    return 0;
}

int RiemannSolver_generic::QuadrantI(const HydroState &state_l,
                                     const HydroState &state_r)
{       
    double Pvac = max(eos_l->P_vac,eos_r->P_vac);
    Wave_r = eos_r->isentrope(state_r);
    if( Wave_r == NULL )
        return -1;          //error
    Wave_r->p_vac = Pvac;
    int status = Wave_r->P(P_l, RIGHT, right_f);
    if( status < 0 )
        return -1;
    if( status > 0 )
    {
       left_f.us = left_f.u - left_f.us;
       return 1;            // cavitation        
    }
    if( u_l > right_f.u )
    { // shock-rarefaction
        Wave_l = eos_l->shock(state_l);
        if( Wave_l == NULL )
            return -1;      // error
        if( Wave_r->u(u_l, RIGHT, right_f) )
            return -1;      // error
        Pmax = right_f.P;
        Pmin = P_l;
        return 0;
    }
    else if( u_l < right_f.u )
    { // rarefaction-rarefaction
        Wave_l = eos_l->isentrope(state_l);
        if( Wave_l == NULL )
            return -1;      // error
        Wave_l->p_vac = Pvac;
        status = Wave_r->u(u_l, RIGHT, right_f);
        if( status < 0 )
            return -1;
        if( status > 0 )
        {
            status = Wave_l->u(right_f.u, LEFT, left_f);
            if( status < 0 )
                return -1;
            if( status > 0 )
                return 1;   // cavitation
        }
        Pmax = P_l;
        Pmin = max(right_f.P,Pvac);
        return 0;
    }
    else
    { // degenerate case: 0 strength left wave
       left_f.us = left_f.u - left_f.us;
       return 1;                 
    }
}

int RiemannSolver_generic::QuadrantII(const HydroState &state_l,
                                      const HydroState &state_r)
{       
    Wave_l = eos_l->shock(state_l);
    if( Wave_l==NULL )
        return -1;      // error
    if( P_c >= P_r )
    {
        if( Wave_l->u(u_c,LEFT,left_f) )
            return -1;  //error
        if( left_f.P >= P_r )
        {
            Wave_r = eos_r->shock(state_r);
            if( Wave_r == NULL || Wave_r->u(u_c,RIGHT,right_f) )
                return -1;  // error
            Pmin = min(left_f.P,right_f.P);
            Pmax = max(left_f.P,right_f.P);
            return 0;            
        }
    }
    if( Wave_l->u(u_r,LEFT,left_f) )
        return -1;  // error
    if( left_f.P < P_r )
    {
        Wave_r = eos_r->isentrope(state_r);
        if( Wave_r == NULL )
            return -1;  // error
        double Z_l =  eos_l->c(left_f)/left_f.V;  // Z = rho*c
        double Z_r = right_f.us/right_f.V;        // Z = rho*c
        u_c  = u_r +(left_f.P-P_r)/(Z_l+Z_r);
        if( Wave_l->u(u_c,LEFT,left_f) )
            return -1;  //error
        Pmax = left_f.P;
        int status = Wave_r->u(u_c,RIGHT,right_f);
        if( status )
        {
        // EOS may have limited domain in expansion
        // try to recover, may fail later with bounds for zero
            Isentrope_ODE *S = dynamic_cast<Isentrope_ODE*>(Wave_r);
            if( S == NULL )
                return -1;
            double V;
            double y[2], yp[2];
            if( S->LastState(V,y,yp) != 1 )  // returns dir
                return -1;
            Pmin = -yp[0];
        }
        else
        {
            Pmin = right_f.P;
        }
        if( Pmin > Pmax )
        {
            Pmax = Pmin;
            Pmin = left_f.P;
        }
        /************
        Pmin = left_f.P;
        Pmax = P_r;             
        int status = Wave_r->P(Pmin,RIGHT,right_f);
        if( status )
        {
        // EOS may have limited domain in expansion
        // try to recover, may fail later with bounds for zero
            Isentrope_ODE *S = dynamic_cast<Isentrope_ODE*>(Wave_r);
            if( S == NULL )
                return -1;
            double V;
            double y[2], yp[2];
            if( S->LastState(V,y,yp) != 1 )  // returns dir
                return -1;
            Pmin = -yp[0];
        }
        ****************/
    }
    else
    {
        Wave_r = eos_r->shock(state_r);
        if( Wave_r == NULL )
            return -1;  // error
        Pmax = left_f.P;
        Pmin = P_r;
    }
    return 0;
}

int RiemannSolver_generic::QuadrantIII(const HydroState &state_l,
                                       const HydroState &state_r)
{       
    Wave_r = eos_r->shock(state_r);
    if( Wave_r==NULL )
        return -1;      // error
    if( P_c >= P_l )
    {
        if( Wave_r->u(u_c,RIGHT,right_f) )
            return -1;  //error
        if( right_f.P >= P_l )
        {
            Wave_l = eos_l->shock(state_l);
            if( Wave_l == NULL || Wave_l->u(u_c,LEFT,left_f) )
                return -1;  // error
            Pmin = min(left_f.P,right_f.P);
            Pmax = max(left_f.P,right_f.P);
            return 0;            
        }
    }
    if( Wave_r->u(u_l,RIGHT,right_f) )
        return -1;  // error
    if( right_f.P < P_l )
    {
        Wave_l = eos_l->isentrope(state_l);
        if( Wave_l == NULL )
            return -1;  //error
        double Z_l = left_f.us/left_f.V;            // Z = rho*c
        double Z_r =  eos_r->c(right_f)/right_f.V;  // Z = rho*c
        u_c  = u_l +(P_l-right_f.P)/(Z_l+Z_r);
        if( Wave_r->u(u_c,RIGHT,right_f) )
            return -1;  //error
        Pmax = right_f.P;
        int status = Wave_l->u(u_c,LEFT,left_f);
        if( status )
        {
        // EOS may have limited domain in expansion
        // try to recover, may fail later with bounds for zero
            Isentrope_ODE *S = dynamic_cast<Isentrope_ODE*>(Wave_l);
            if( S == NULL )
                return -1;
            double V;
            double y[2], yp[2];
            if( S->LastState(V,y,yp) != 1 )  // returns dir
                return -1;
            Pmin = -yp[0];
        }
        else
        {
            Pmin = left_f.P;
        }
        if( Pmin > Pmax )
        {
            Pmax = Pmin;
            Pmin = right_f.P;
        }
        /************
        Pmin = right_f.P;
        Pmax = P_l; 
        int status = Wave_l->P(Pmin,LEFT,left_f);
        if( status )
        {
        // EOS may have limited domain in expansion
        // try to recover, may fail later with bounds for zero
            Isentrope_ODE *S = dynamic_cast<Isentrope_ODE*>(Wave_l);
            if( S == NULL )
                return -1;
            double V;
            double y[2], yp[2];
            if( S->LastState(V,y,yp) != 1 )  // returns dir
                return -1;
            Pmin = -yp[0];
        }
        ****************/
    }
    else
    {
        Wave_l = eos_l->shock(state_l);
        if( Wave_l == NULL )
            return -1;  // error
        Pmax = right_f.P;
        Pmin = P_l;
    }
    return 0;
}

int RiemannSolver_generic::QuadrantIV(const HydroState &state_l,
                                      const HydroState &state_r)
{       
    double Pvac = max(eos_l->P_vac,eos_r->P_vac);
    Wave_l = eos_l->isentrope(state_l);
    if( Wave_l == NULL )
        return -1;          // error
    Wave_l->p_vac = Pvac;
    int status = Wave_l->P(P_r, LEFT, left_f);
    if( status < 0 )
        return -1;
    if( status > 0 )
    {
       right_f.us += right_f.u;
       return 1;            // cavitation        
    }
    if( u_r < left_f.u )
    { // rarefaction-shock
        Wave_r = eos_r->shock(state_r);
        if( Wave_r == NULL )
            return -1;      // error
        if( Wave_l->u(u_r, LEFT, left_f) )
            return -1;      // error
        Pmin = P_r;
        Pmax = left_f.P;
        return 0;
    }
    else if( u_r > left_f.u )
    { // rarefaction-rarefaction
        Wave_r = eos_r->isentrope(state_r);
        if( Wave_r == NULL )
            return -1;      // error
        Wave_r->p_vac = Pvac;
        status = Wave_l->u(u_r, LEFT, left_f);
        if( status < 0 )
            return -1;
        if( status > 0 )
        {
            status = Wave_r->u(left_f.u, RIGHT, right_f);
            if( status < 0 )
                return -1;
            if( status > 0 )
                return 1;   // cavitation
        }
        Pmax = P_r;
        Pmin = max(left_f.P,Pvac);
        return 0;
    }
    else
    { // degenerate case: 0 strength left wave
       right_f.us += right_f.u;
       return 1;                 
    }
}
