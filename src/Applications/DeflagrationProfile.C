#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn.h>
#include <Arrhenius.h>
#include <EOS_VT.h>
#include <PTequilibrium.h>
#include <PTequilibrium_VT.h>
//
ostream& PrintState(EOS &eos, WaveState &state)
{
    double c  = sqrt(eos.c2(state.V,state.e));
    double T  = eos.T(state.V,state.e);
    double cT = sqrt(state.V*eos.KT(state.V,state.e));
    cout << state  << " "
         << c_form << c  << " "
         << T_form << T  << " "
         << c_form << cT ;
    return cout;
}
void PrintUnits(EOS &eos)
{
    const Units *u = eos.UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u) << " "
            << Center(u_form,u->Unit("velocity"))    << " "
            << Center(T_form,u->Unit("temperature")) << " "
            << Center(u_form,u->Unit("velocity"))    << "\n";
    }
}
void PrintLabels()
{
    WaveStateLabel(cout) << " "
                         << Center(c_form, "c")  << " "
                         << Center(T_form, "T")  << " "
                         << Center(c_form, "cT") << "\n";
}
void PrintLine()
{
    cout.fill('-');
    cout << setw(V_form.width)  << "" << "-"
         << setw(e_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(P_form.width)  << "" << "-"
         << setw(us_form.width) << "" << "-"
         << setw(c_form.width)  << "" << "-"
         << setw(T_form.width)  << "" << "-"
         << setw(c_form.width)  << "" << "\n";
    cout.fill(' ');
}
//
//
class DefProfile : public ODE
{
// Deflagration profile determined by ODEs
// xi = x - D*t <= 0, right facing wave
//    ahead  state, xi =  infty, lambda=0; V0,P0,T0,u0
//    behind state, xi = -infty, lambda=1; V1,P1,T1,u1
// zeta = xi/ell,   dimensionless length scale
// where
//   ell   = kappa/(m*C_ref), diffusion length scale
//   m     = rho*(D-u),       mass flux
//   C_ref = reference specific heat
// Pr   = C_ref*nu/kappa, Prandtl number
// start trajectory at zeta = 0, and integrate to zeta < 0
//
// (d/dzeta) lambda = - [ell/(V*m)] * Rate
// (d/dzeta) T      = - {Delta[e]-[P0+0.5*m^2*(V0-V)]*(V_0-V)} / C_ref
// (d/dzeta) V      = - Pr^{-1} * Delta[P+m^2*V] / m^2
// (d/dzeta) Th
// (d/dzeta) Vh
//      (Vh,Th) = intersection of partly burned Hugoniot & Rayleigh line
//                that is, right hand side vanishes for (d/dzeta) (T & V)
// where
//   m = rho*(D-u) is mass flux
//   kappa is coefficient of thermal conduction
//     dimensions (energy/time)/length/K
//     Heat flux, q = -kappa*(d/dx)T, dimensions (energy/time)/length^2
//   nu is coefficient of dynamic viscosity
//     dimensions pressure*time
//     Viscous pressure, Q = -nu*(d/dx)u
public:
    void set_lambda(double lambda);
public:
    Arrhenius *rate;
    PTequilibrium_VT *HE;
    EOS_VT *reactants;
    EOS_VT *products;
    DefProfile(Arrhenius *Arate, double Kappa, double Nu);
    ~DefProfile();
    int Init(double lambda_0,   double V_0,   double T_0,
             double wave_speed, double u_0,
             double V_max,      double T_max, double dV = 0.0);
    double kappa;       // coef. of thermal conduction
    double nu;          // coef. of (dynamic) viscosity
    double C_ref;       // reference specific heat
    double Pr;          // Prandtl number = C_ref*nu/kappa
    double D,u0;        // Deflagration wave speed and ahead velocity
    double m, m2;       // mass flux, m = (D-u0)/V0
    double length;      // diffusion length scale = kappa/(m*C_ref)
    double V0, T0, lambda0;
    double P0,e0;
    double Vmax, Tmax;  // bounds on trajectory: V0 < V < Vmax, T0 < T < Tmax
    double y[5],yp[5];
    int F(double *yp_z, const double *y_z, double zeta); // ODE::F 
    double StopCondition(const double *y_z, const double *yp_z, double zeta);
    double MaxNorm(const double *y0, const double *y1,
                   const double*,    const double*);
    int Vend(double V, double &zeta, double *y_z=NULL, double *yp_z=NULL);
    double P(double y_z[3]);
    //
    void Anr(double Vc, double Tc, double AV[2], double AT[2]);
    void Ah(double Vh, double Th, double AV[2], double AT[2]);
    void Eigen(double AV[2], double AT[2], double e[2], double dTdV[2]);
    double dTdVh(double Vh, double Th);
    double  Rate(double lambda, double T, double V);
    void A_lambda(double lambda, double T, double V, double a[2]);
};
void DefProfile::A_lambda(double lambda, double Tc, double Vc, double a[2])
{   // lambda column of matrix at critical point
    set_lambda(max(min(lambda,1.-1.e-12),1e-12));
    double Pc  = HE->P(Vc,Tc);
    double Vp  = HE->V2;
    double hp  = products->e(Vp,Tc)+Pc*Vp;
    double Kp  = products->KT(Vp,Tc);
    double Gp  = products->Gamma(Vp,Tc);
    double Cvp = products->CV(Vp,Tc);
    //
    double Vr  = HE->V1;
    double hr  = reactants->e(Vr,Tc)+Pc*Vr;
    double Kr  = reactants->KT(Vr,Tc);
    double Gr  = reactants->Gamma(Vr,Tc);
    double Cvr = reactants->CV(Vr,Tc);
    //
    // a[0] = (dP/dlambda)_V,T
    // a[1] = (de/dlambda)_V,T
    double den = lambda*Vp/Kp+(1.-lambda)*Vr/Kr;
    a[0] = (Vp-Vr)/den;
    a[1] = (hp-hr) - (lambda*Gp*Cvp/Kp+(1.-lambda)*Gr*Cvr/Kr)*Tc*a[0];
    // scale
    a[0] *= 1./(m2*Vc*Pr);
    a[1] *= 1./(C_ref*Tc);
}
double DefProfile::Rate(double lambda, double T, double V)
{   // scaled rate
    return length*rate->Rate(lambda,T)/(m*V);
}
double DefProfile::P(double y_z[3])
{
    double lambda = y_z[0];
    double T      = y_z[1];
    double V      = y_z[2];
    set_lambda(lambda);
    return HE->P(V,T);
}
DefProfile::DefProfile(Arrhenius *Arate, double Kappa, double Nu)
    : ODE(5,512), rate(Arate), kappa(Kappa), nu(Nu)
{
    if( rate == NULL )
        cerr << Error("bad argument to DefProfile") << Exit;
    if( rate->HE == NULL )
        cerr << Error("rate->HE") << Exit;
    HE = dynamic_cast<PTequilibrium_VT *>(rate->HE->eosVT());
    if( HE == NULL )
        cerr << Error("HE_VT is NULL") << Exit;
    reactants = HE->eos1;
    products  = HE->eos2;
}
DefProfile::~DefProfile()
{
    EOS_VT *eosVT = HE;
    deleteEOS_VT(eosVT);
}
void DefProfile::set_lambda(double lambda)
{
        HE->lambda1 = 1.-lambda;
        HE->lambda2 = lambda;
}
void DefProfile::Anr(double Vc, double Tc, double AV[2], double AT[2])
{
    // returns non-reactive submatrix at critical point (Vc,Tc)
    // matrix Anr = (AV,AT)^T
    double KT    = HE->KT(Vc,Tc);
    double Gamma = HE->Gamma(Vc,Tc);
    double Cv    = HE->CV(Vc,Tc);
    AV[0] = (1. - KT/(m2*Vc))/Pr;
    AV[1] = Gamma*Cv*Tc/sqr(m*Vc)/Pr;
    AT[0] = Gamma*Cv/C_ref;
    AT[1] = Cv/C_ref;    
}
void DefProfile::Ah(double Vh, double Th, double AV[2], double AT[2])
{
    // returns derivative matrix for partly burned Hugoniot at (Vh,Th)
    // matrix Ah = (AV,AT)^T
    double cT2   = HE->cT2(Vh,Th);
    double Gamma = HE->Gamma(Vh,Th);
    double Cv    = HE->CV(Vh,Th);
    AV[0] = m2 - cT2/(Vh*Vh);
    AV[1] = (Gamma/Vh)*Cv;
    AT[0] = AV[1]*Th;
    AT[1] = Cv;    
}
void DefProfile::Eigen(double AV[2], double AT[2], double e[2], double dTdV[2])
{
    // returns eigenvalues and eigenfunctions
    // matrix A = (AV,AT)^T
    // det(A - y*I) = 0
    // y^2 - Tr(A)*y + det(A) = 0
    // y^2 -   2*b * y + c      = 0
    double b = 0.5*(AV[0]+AT[1]);
    double c = AV[0]*AT[1] - AV[1]*AT[0];
    double dsc = sqrt(b*b - c);
    e[0] = b-dsc;  // negative eigenvalue
    e[1] = b+dsc;  // positive eigenvalue
    // eigenvector = (1,dTdV)
    int i;
    for( i=0; i<2; i++ )
    {
        double y = e[i];
        if( abs(1.-AV[0]/y) > abs(1.-AT[1]/y) )
            dTdV[i] = (y-AV[0])/AV[1];
        else
            dTdV[i] = AT[0]/(y-AT[1]);
    }
    // remember, dTdV *= Tc/Vc;
}

double DefProfile::dTdVh(double Vh, double Th)
{
    // dT/dV on Hugoniot locus at (Vh,Th)
    double P     = HE->P(Vh,Th);
    double KT    = HE->KT(Vh,Th);
    double Gamma = HE->Gamma(Vh,Th);
    double Cv    = HE->CV(Vh,Th);
    return (0.5*((1.-V0/Vh)*KT+P-P0)-Gamma*Cv*Th/Vh)
           / (Cv*(1.+0.5*(1.-V0/Vh)*Gamma));
}

int DefProfile::Init(double lambda_0, double V_0,   double T_0,
                     double w_speed,  double u_0,
                     double V_max,    double T_max, double dV)
{
    y[0] = lambda0 = lambda_0;
    y[1] = T0      = T_0;
    y[2] = V0      = V_0;
    y[3] = T0;
    y[4] = V0;
    u0 = u_0;
    D = w_speed;
    Vmax = V_max;
    Tmax = T_max;
    m = (D-u0)/V0;
    m2 = m*m;
    //
    set_lambda(lambda0);
    P0 = HE->P(V0,T0);
    e0 = HE->e(V0,T0);
    C_ref  = HE->CP(V0,T0);
    Pr     = C_ref*nu/kappa;
    length = kappa/(m*C_ref);
    //
    //  Eigenvalues and functions at initial state
    //
    double AV[2], AT[2];
    Anr(V0,T0,AV,AT);
    double e[2], dTdV[2];
    Eigen(AV, AT, e, dTdV);
    dTdV[1] *= T0/V0;
    y[1] += dV*dTdV[1];
    y[2] += dV;
    //
    double rate0 = max(rate->Rate(lambda0,y[1]),0.01);
    double dzeta = 1.e-4*(V0*m/length)/rate0;
    double zeta0 = 0.0;
    int status = Initialize(zeta0, y, -dzeta); 
    return status;
}
double DefProfile::StopCondition(const double *y_z, const double *yp_z,
                                       double zeta)
{
    // bounds on trajectory:
    //      V0 < V < Vmax
    //      T0 < T < Tmax
    //      (d/dzeta)T < 0 and (d/dzeta)V < 0
    double T = y_z[1];
    double V = y_z[2];
    double stop = min( min(V-0.9*V0, 1.001*Vmax-V),
                       min(T-0.9*T0, 1.001*Tmax-T) );
    // avoid noise at start of profile
    if( V > 0.5*(V0+Vmax) )
        stop = min(stop, min(-yp_z[1], -yp_z[2]) );
    return stop;
}
int DefProfile::F(double *yp_z, const double *y_z, double zeta)
{
    double lambda = y_z[0];
    if( lambda >= 1.0 )
    {
        yp_z[0] = 0.0;
        yp_z[1] = 0.0;
        yp_z[2] = 0.0;
        yp_z[3] = 0.0;
        yp_z[4] = 0.0;
        return 0;
    }
    double T      = y_z[1];
    double V      = y_z[2];
    double Th     = y_z[3];
    double Vh     = y_z[4];
    set_lambda(lambda);
    double P = HE->P(V,T);
    if( isnan(P) )
    {
        //cout << "debug: P isnan\n";
        return 1;
    }
    double e = HE->e(V,T);
    //double dB  = e - (e0 - (P0 -0.5*m2*(V-V0))*(V-V0));
    //double dB  = (e - e0) + (P0 -0.5*m2*(V-V0))*(V-V0);
    double dB  = (e  + P0*(V-V0)) -(e0+0.5*m2*(V-V0)*(V-V0));
    //double dRL = P - (P0 +m2*(V0-V));
    //double dRL = (P - P0) +m2*(V-V0);
    double dRL = (P +m2*(V-V0)) - P0;
    yp_z[0] = - length*rate->Rate(lambda,T)/(m*V);
    yp_z[1] = - dB / C_ref;
    yp_z[2] = - (dRL/m2)/Pr;
    // temporary fix to guarantee HE->V1 and HE->V2 are calculated
    if( lambda <= 0.0 )
    {
        lambda = 1e-10;
        set_lambda(lambda);
    }
    //
    set_lambda(lambda);
    double Ph = HE->P(Vh,Th);       // = P0 - m2*(Vh-V0)
    double eh = HE->e(Vh,Th);
    //
    double cT2   = HE->cT2(Vh,Th);
    double Gamma = HE->Gamma(Vh,Th);
    double Cv    = HE->CV(Vh,Th);
    // 1 -> V  and 2->T
    double A11 = m2 - cT2/(Vh*Vh);
    double A12 = (Gamma/Vh)*Cv;
    double A21 = A12*Th;
    double A22 = Cv;
    //
    double Vp  = HE->V2;
    double hp  = products->e(Vp,Th)+Ph*Vp;
    double Kp  = products->KT(Vp,Th);
    double Gp  = products->Gamma(Vp,Th);
    double Cvp = products->CV(Vp,Th);
    //
    double Vr  = HE->V1;
    double hr  = reactants->e(Vr,Th)+Ph*Vr;
    double Kr  = reactants->KT(Vr,Th);
    double Gr  = reactants->Gamma(Vr,Th);
    double Cvr = reactants->CV(Vr,Th);
    //
    double den = lambda*Vp/Kp+(1.-lambda)*Vr/Kr;
    // S1 = (dP/dlambda)_V,T > 0
    double S1 = (Vp-Vr)/den;
    //double S2 =-(hp-hr-(lambda*Gp*Cvp/Kp+(1.-lambda)*Gr*Cvr/Kr)*(Vp-Vr)*Th/den);
    // S2 = (de/dlambda)_V,T < 0
    double S2 = (hp-hr) - (lambda*Gp*Cvp/Kp+(1.-lambda)*Gr*Cvr/Kr)*Th*S1;
    // correct cumulative integration errors
    double dP = Ph - (P0 + m2*(V0-Vh));
    double de = eh - (e0+0.5*(Ph+P0)*(V0-Vh));
    //S1 *= 1.+dP*Vh/cT2;
    //S2 *= 1.-de/(Cv*Th);
    S1 += 100.*dP;
    S2 += 100.*de;
    //
    double det = A11*A22 - A12*A21;
    yp_z[4] = -yp_z[0]*(S1*A22-S2*A12)/det;  // dVh/dzeta
    yp_z[3] = -yp_z[0]*(A11*S2-A21*S1)/det;  // dTh/dzeta 
    return 0;
}
double DefProfile::MaxNorm(const double *y0, const double *y1,
                        const double*, const double*)
{
    double Norm = 0;
    for(int i=Dimension(); i--; y0++, y1++)
        Norm = max(Norm, abs(*y0 - *y1)/((abs(*y0)+abs(*y1) + 1.)*epsilon));
    return Norm;
}
class V_ODE : public ODEfunc
{
public:
    double Vmin;
    int count;
    V_ODE(double Vcut) : Vmin(Vcut) {count = 0;}
    double f(double V, const double *y, const double *yp);
};
double V_ODE::f(double t, const double *y, const double *yp)
{
/*************** debug
if( ++count == 10 )
{
double lambda = y[0];
double T = y[1];
double V = y[2];
double dTdz = yp[1];
double dVdz = yp[2];
double zeta = -t;
cerr << "V_ODE::f zeta lambda V T dVdz dTdz "
     << zeta << " " << lambda << " " << V << " " << T << " "
     << dVdz << " " << dTdz << "\n";
count = 0;
}
*************/
    if( y[2]<Vmin ) return -Vmin;
    return -y[2];   // -V
}
int DefProfile::Vend(double V, double &zeta, double *y_z, double *yp_z)
{
    if( y_z==NULL )
        y_z = y;
    if( yp_z==NULL )
        yp_z = yp;
    V_ODE Vode(0.5*(V+V0));
    V = -V;
    int status = Integrate(Vode,V,zeta,y_z,yp_z);
    return status;  
}
//
//
int RunProfile( double V0, double T0, double D,
                Deflagration *defl, DefProfile &profile, double dV,
                int debug )
{
    // return value = -1    // error
    //              =  0    // D too large, reached V_defl & T > T_defl
    //              =  1    // D too small,  V < V_defl, dV/dzeta > 0
    WaveState D_state;  // state behind deflagration wave
    if( defl->u_s(D,RIGHT,D_state) )
    {
        WaveState CJ_state;
        defl->CJwave(RIGHT,CJ_state);
        cerr << "Dcj = " << CJ_state.us << "\n";
        cerr << Error("defl_us failed") << Exit;
    }
    double V1, T1;
    V1 = D_state.V;
    EOS &eos = defl->Eos();
    T1 = eos.T(V1, D_state.e);
    //
    double lambda0 = 0.0;
    double u0 =0.0;
    double *y  = profile.y;
    double *yp = profile.yp;
    //profile.IgnoreStopOff();
    int status = profile.Init(lambda0,V0,T0,D,u0, V1,T1, dV);
    if( status )
        cerr << Error("Profile.init failed with status ")
             << profile.ErrorStatus(status) << Exit;
    double V = V1;
    double zeta = 0.0;
    {   //debug
        // Near ahead state, step size dzeta may be very small
        // depends on dVfact and kappa
        // The following has effect of increasing ODE:max_steps
        // But checks that some progress is being made
        for( ; zeta > -4.; zeta -= 0.5 )
        {
            status = profile.Integrate(zeta,y,yp);
            if( ! (status == 0 || status == STOP_CONDITION) )
                break;
            double lambda = y[0];
            /****************
            double ODE_dzeta = -profile.StepSize();
            double T = y[1];
            double V = y[2];
            cerr << "debug: status " << status
                 << " zeta " << zeta
                 << " dzeta " << ODE_dzeta
                 << " lambda " << lambda
                 << " V " << V
                 << " T " << T
                 << "\n";
            *****/
            if( lambda > 0.2 )
                break;
        }
    }
    //profile.IgnoreStopOn();
    if( status == 0 )
        status = profile.Vend(V,zeta);
    if( ! (status == 0 || status == STOP_CONDITION) )
    {
        /**** debug **/
        double dTdzeta = yp[1];
        double dVdzeta = yp[2];
        double T = y[1];
        double V = y[2];
        cerr << "status " << status << " D, zeta= " << D << " " << zeta << "\n"
             << "\tV,T = " << V << " " << T
             << ", dVdz, dTdz = " << dTdzeta << " " << dVdzeta << "\n";
        // status = profile.Vend(V,zeta);
        /*****/
        status = -1;    // for debugging
    }
    else
    {
        double dTdzeta = yp[1];
        double dVdzeta = yp[2];
        double T = y[1];
        double V = y[2];
        if( T > T1 )
            status = 0;     // D too large
        else if( V <= V1 )
            status = 1;     // D too small
        else
        {
            /**** debug **/
            cerr << "status " << status << " D, zeta= " << D << " " << zeta << "\n"
                 << "\tV,T = " << V << " " << T
                 << ", dVdz, dTdz = " << dTdzeta << " " << dVdzeta << "\n";
            // status = profile.Vend(V,zeta);
            /****/
            status = -1;   // for debugging
        }
    }
    if( (debug>0) || (debug<0 && status==-1) )
    {
        cerr << "D=" << D << ", status " << status << "\n";
        cerr << " zeta=" << zeta  << "\n";
        cerr << " lambda=" << y[0] 
                  << " (T-T0)/(T_defl-T0)=" << (y[1]-T0)/(T1-T0)
                  << " (V-V0)/(V_defl-V0)=" << (y[2]-V0)/(V1-V0)
                  << "\n";
        cerr << "   yp=" << yp[0] 
                  << " " << yp[1]
                  << " " << yp[2] << "\n";
    }
    return status;
}
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = NULL;
    const char *type     = "HEburn";
    const char *name     = NULL;
    const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    double epsilon = 1.e-6;     // ODE tolerance
    double dzeta = 0.5;         // spatial step size for printout of profile 
    int nsteps = 20;            // number of steps for printout of profile
    int Biter = 20;             // number of iterations for bisection of D
    int count  = 3;             // Number of Newton iterations for H(lambda)
    //
    double Ps = 0.0;            // initial shock pressure
    double D  = 0.0;            // Deflagration speed
    double Dmax = 0.0;          // upper bound for iteration to determine D
    double Dmin = 0.0;          // lower bound for iteration to determine D
    double kappa = 0.0;         // coefficient of thermal conduction
                                //   heat flux = -kappa*(d/dx)T
                                //   k = kappa/(rho*Cp)
                                //     = coefficient of thermal diffusion
                                //   dimensions of length^2/time
    double nu = 0.0;            // coefficient of dynamic visocity
                                //   Viscous pressure = -nu*(d/dx)u
                                //   dimension of nu, pressure*time
    double prandtl = 1.0;       // Prandtl number = Cp*nu/kappa
    double dVfact  = 1.e-3;     // offset from critical point
    int evalD = 0;              // flag to skip iteration to determine D
    int debug = 0;              // flag for diagnostic print out
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        //GetVar(type,type);
        GetVar(material,material);
        GetVar(lib,lib);
        GetVar(units,units);
        GetVar(epsilon,epsilon);
        GetVar(dzeta,dzeta);
        GetVar(nsteps,nsteps);
        GetVar(Biter,Biter);
        GetVar(count,count);
        //
        GetVar(Ps,Ps);
        GetVar(D,D);
        GetVar(Dmin,Dmin);
        GetVar(Dmax,Dmax);
        GetVar2(Drange,Dmin,Dmax);
        GetVar(kappa,kappa);
        GetVar(nu,nu);
        GetVar(prandtl,prandtl);
        GetVar(dVfact,dVfact);
        GetVarValue(evalD,evalD,1);
        GetVar(debug,debug);
        ArgError;
    }
    //
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    //
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    if( material )
    {
        if( TypeName(material,type,name) )
            cerr << Error("syntax error, material = ") << material
                 << Exit;
        if( strcmp(type,"HEburn") )
            cerr << Error("material type not HEburn") << Exit;
    }
    if( name==NULL )
        cerr << Error("must specify name or material") << Exit;
    if( kappa <= 0.0 )
        cerr << Error("must specify kappa > 0") << Exit;
    if( lib )
#ifdef LINUX
      setenv("SharedLibDirEOS",lib,1);
#else
      putenv(Cat("SharedLibDirEOS=",lib));
#endif
    //
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *Reactants = HE->Reactants();
    if( Reactants == NULL )
        cerr << Error("Reactants is NULL") << Exit;
    EOS *Products = HE->Products();
    if( Products == NULL )
        cerr << Error("Products is NULL") << Exit;
    Arrhenius *Rate =  dynamic_cast<Arrhenius *>(HE->rate);
    if( Rate == NULL )
        cerr << Error("HE->rate not Arrhenius") << Exit;
    //
    double Vref = HE->V_ref; 
    double eref = HE->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    HydroState state0(Vref,eref);
    //
    if( Ps <= Pref )
        cerr << Error("Ps <= Pref") << Exit;
    Hugoniot *hug = Reactants->shock(state0);
    if( hug==NULL )
        cerr << Error("Reactants->shock failed" ) << Exit;
    WaveState S_state;          // shock state = state ahead of deflagration
    if( hug->P(Ps,RIGHT,S_state) )
        cerr << Error("hug->P failed") << Exit;
    WaveState A_state = S_state;    // state ahead of deflagration
    A_state.u = 0.0;                // rest frame of state behind shock   
    Deflagration *defl =  Products->deflagration(A_state,A_state.P);
    if( defl==NULL )
        cerr << Error("Products->deflagration failed" ) << Exit;
    WaveState CJ_state;
    if( defl->CJwave(RIGHT,CJ_state) )
         cerr << Error("CJwave failed") << Exit;
    double C_ref = Reactants->CP(A_state.V,A_state.e);
    if( nu <= 0.0 )
    {
        nu = prandtl*kappa/C_ref;
        cout << "kappa="     << kappa
             << ", nu="      << nu << " (*1e4 for Poise)"
             << ", prandtl=" << prandtl
             << "\n";
    }
    else
        prandtl = nu*C_ref/kappa;
    DefProfile profile(Rate,kappa,nu);
    profile.epsilon = epsilon;
    double lambda0 = 0.0;
    double V0 = A_state.V;
    double T0 = Reactants->T(A_state.V,A_state.e);
    double u0 = A_state.u;
    if( D <= 0.0 )
    {
        // estimate D
        if( Dmin > 0.0 || Dmax > 0.0 )
          D = 0.5*(Dmin+Dmax);
        else
        {
            double lambda = 0.5;
            double T = 0.8*Products->T(CJ_state)+0.2*Reactants->T(A_state);
            double rate = profile.rate->Rate(lambda,T);
            D = sqrt(kappa*A_state.V*rate/C_ref);
            //temporary fix for roundoff errors with integrand
            D = min(D, 0.8*CJ_state.us);
            D = max(D,0.05*CJ_state.us);
        }
        cout << "initial guess, D = " << D << "\n";
    }
    //
    // find bounds on D for bisection iteration
    //
    int i;
    int status;
    double zeta, y[5],yp[5];
    double dV = dVfact*(CJ_state.V-A_state.V);
    if( !evalD )
    {
        if( Dmin <= 0.0 || Dmax <= 0.0 )
        {
            status = RunProfile(V0, T0, D, defl, profile, dV, -1);
            if( status == -1 )
                cerr << Error("initial D failed with status ")
                     << Exit;
            if( status==0 )
            {
                Dmax = D;
                Dmin = 0.5*Dmax;
                for(i=10; i>0; i--)
                {
                    status = RunProfile(V0,T0, Dmin, defl,profile, dV,debug==1);
                    if( status==0 )
                    {
                        Dmax = Dmin;
                        Dmin = 0.5*Dmax;
                    }
                    else if( status==1 )
                        break;
                    else
                        Dmin = 0.5*(Dmax+Dmin);
                }
            }
            else
            {
                Dmin = D;
                Dmax = 0.5*(Dmin+CJ_state.us);
                for(i=10; i>0; i--)
                {
                    status = RunProfile(V0,T0, Dmax, defl,profile, dV,debug==1);
                    if( status==0 )
                        break;
                    else if( status==1)
                    {
                        Dmin = Dmax;
                        Dmax = 0.5*(Dmin+CJ_state.us);                  
                    }
                    else
                        Dmax = 0.5*(Dmin+Dmax);            
                }
            }
            if( i==0 )
                cerr << Error("failed to find bounds on D") << Exit;
        }
        // bisect to find D
        for( i=Biter; i>0; i-- )
        {
            D = 0.5*(Dmin+Dmax);
            status = RunProfile(V0,T0, D, defl,profile, dV, debug==1);
            if( status == 0 )
                Dmax = D;
            else
                Dmin = D;
            double lambda = profile.y[0];
            if( lambda > 0.999 )
                break;
        }
        cout << "bracket, (" << Dmin << "," << Dmax << "): D=" << D << "\n";
        //D = 0.5*(Dmin+Dmax);
        if ( debug == 1)
          cerr << Error("debug") << Exit;
    }
    //
    //
    WaveState D_state;  // state behind deflagration wave
    if( defl->u_s(D,RIGHT,D_state) )
         cerr << Error("defl_us failed") << Exit;
    double Tdefl = Products->T(D_state.V,D_state.e);
    if( 0 ) // check VT EOS
    {
        double Pdefl = D_state.P;
        double edefl = D_state.e;
        double P_VT = profile.products->P(D_state.V,Tdefl);
        double e_VT = profile.products->e(D_state.V,Tdefl);
        cerr << "EOS_VT test: dP/P=" << (P_VT-Pdefl)/Pdefl << "\n";
        cerr << "EOS_VT test: de/e=" << (e_VT-edefl)/edefl << "\n";
    }
    // print header
    cout << type << "::" << name << "\n";
    PrintState(*Reactants, S_state) << " shock state\n";
    PrintState(*Reactants, A_state) << " ahead state\n";
    PrintState(*Products, CJ_state) << "    CJ state\n";
    PrintLine();
    PrintLabels();
    PrintUnits(*eos);
    PrintLine();
    PrintState(*Products, D_state) << " deflagration\n";
    status = profile.Init(lambda0,V0,T0,D,u0, D_state.V,Tdefl, dV);
    if( debug == 5 )
    { // Eigen-functions at end states
        double AV[2], AT[2];
        double e[2],eT[2];
        double lambda, Tc,Vc;
        // Ahead state
        cout << "Ahead state\n";
        Vc = A_state.V;
        Tc = Reactants->T(A_state.V,A_state.e);
        lambda = 0.0;
        profile.set_lambda(lambda);
        profile.Anr(Vc,Tc,AV,AT);
        profile.Eigen(AV,AT,e,eT);
        cout << "\t AV: " << AV[0] << "  " << AV[1] << "\n";
        cout << "\t AT: " << AT[0] << "  " << AT[1]
             << "\t\t\t det = " << AV[0]*AT[1] -AT[0]*AV[1] << "\n";
        cout << "\t eigenvalue:  " << e[1] << " dT/dV= " << eT[1]*Tc/Vc
             << "\t det = " << (AV[0]-e[1])*(AT[1]-e[1])-AT[0]*AV[1] << "\n";
        cout << "\t eigenvalue:  " << e[0] << " dT/dV= " << eT[0]*Tc/Vc
             << "\t det = " << (AV[0]-e[0])*(AT[1]-e[0])-AT[0]*AV[1] << "\n";
        // Behind state
        cout << "Behind state\n";
        Vc = D_state.V;
        Tc = Products->T(D_state.V,D_state.e);
        double r3 = -profile.Rate(lambda,Tc,Vc);
        lambda = 1.0;
        profile.set_lambda(lambda);
        profile.Anr(Vc,Tc,AV,AT);
        // 
        profile.Eigen(AV,AT,e,eT);
        cout << "\t AV: " << AV[0] << "  " << AV[1] << "\n";
        cout << "\t AT: " << AT[0] << "  " << AT[1]
             << "\t\t\t det = " << AV[0]*AT[1] -AT[0]*AV[1] << "\n";
        cout << "\t eigenvalue:  " << e[0] << " dT/dV= " << eT[0]*Tc/Vc
             << "\t det = " << (AV[0]-e[0])*(AT[1]-e[0])-AT[0]*AV[1] << "\n";
        cout << "\t eigenvalue:  " << e[1] << " dT/dV= " << eT[1]*Tc/Vc
             << "\t det = " << (AV[0]-e[1])*(AT[1]-e[1])-AT[0]*AV[1] << "\n";
        double a[2];
        profile.A_lambda(lambda,Tc,Vc,a);
        cout << "\t r3= " << r3 <<  ", a= " << a[0] << " " << a[1] << "\n";
        //
        AV[0] -= r3;
        AT[1] -= r3;
        double det = AV[0]*AT[1] - AT[0]*AV[1];
        double dV = -(a[0]*AT[1] -a[1]*AV[1])/det;
        double dT = -(AV[0]*a[1]-AT[0]*a[0])/det;
        cout << "\t eigenvector: (" << Vc*dV << ", " << Tc*dT << ", 1)\n";     
        cout << "      length scale = " << profile.length << "\n";
        cout << "Deflagration speed = " << D << "\n";
        cerr << Error("debug") << Exit;
    }
    //
    //
    //profile.IgnoreStopOn();
    if( status )
        cerr << Error("Profile.init failed with status ")
             << profile.ErrorStatus(status) << Exit;
    cout.setf(ios::left, ios::adjustfield);
    //cout << "\nDeflagration profile: D=(" << Dmin << "," << Dmax << ")\n";
    cout << "      length scale = " << profile.length << "\n";
    cout << "Deflagration speed = " << D << "\n";
    cout        << setw(11) << "#  zeta"
         << " " << setw(11) << "   lambda"
         << " " << setw(11) << "   u"
         << " " << setw(11) << "   V"
         << " " << setw(11) << "   T"
         << " " << setw(11) << "   P"
         << " " << setw(11) << "   Vh"
         << " " << setw(11) << "   Th"
         << " " << setw(9) << "   dzeta"
         << " " << setw(9) << "dlambdadz"
         << " " << setw(9) << "   dVdz"
         << " " << setw(9) << "   dTdz"
         << " " << setw(9) << "   dVhdz"
         << " " << setw(9) << "   dThdz"
         << "\n";
    cout.setf(ios::right, ios::adjustfield);
    int last = 0;
    double lambda, u, T, V, P, Th, Vh;
    for( i=0; i<=nsteps; i++ )
    {
        zeta = -i*dzeta;
        if( last )
        {
            V = D_state.V;
            status = profile.Vend(V,zeta,y,yp);
            if( status )
                break;
        }
        else
        {
            status = profile.Integrate(zeta,y,yp);
            if( status )
            {
                profile.LastState(zeta,y,yp);
                last = 1;
            }
            if( y[2] > D_state.V )
            {
                last = 1;
                V = D_state.V;
                status = profile.Vend(V,zeta,y,yp);
            }
        }
        lambda = y[0];
        T      = y[1];
        V      = y[2];
        P = profile.P(y);
        u = D - profile.m*V;
        Th = y[3];
        Vh = y[4];
        if( lambda > 0.0 )
        { // count Newton-Raphson iteratation
          // to correct (Vh,Th) state on partly burned Hugoniot loci
            PTequilibrium_VT *HE = profile.HE;
            int k = max(count,0);
            while(k--)
            {
                double Ph = HE->P(Vh,Th);
                double dP = Ph - (profile.P0 + profile.m2*(V0-Vh));
                double eh = profile.HE->e(Vh,Th);
                double de = eh - (profile.e0+0.5*(Ph+profile.P0)*(V0-Vh));
                //
                double AV[2], AT[2];
                profile.Ah(Vh,Th,AV,AT);   
                //
                double det = AV[0]*AT[1] - AV[1]*AT[0];
                double S1 = dP;
                double S2 = de;
                Vh -= (S1*AT[1]-S2*AV[1])/det;
                Th -= (AV[0]*S2-AT[0]*S1)/det;
            }
        }
        double ODE_dzeta = -profile.StepSize();
        cout        << setw(11) << setprecision(4) << zeta
             << " " << setw(11) << setprecision(4) << lambda
             << " " << setw(11) << setprecision(4) << u
             << " " << setw(11) << setprecision(4) << V
             << " " << setw(11) << setprecision(4) << T
             << " " << setw(11) << setprecision(4) << P
             << " " << setw(11) << setprecision(4) << Vh
             << " " << setw(11) << setprecision(4) << Th
             << " " << setw(9)  << setprecision(2) << ODE_dzeta
             << " " << setw(9)  << setprecision(2) << yp[0]
             << " " << setw(9)  << setprecision(2) << yp[2]
             << " " << setw(9)  << setprecision(2) << yp[1]
             << " " << setw(9)  << setprecision(2) << yp[4]
             << " " << setw(9)  << setprecision(2) << yp[3]
             << "\n";
        if( debug == 4 )
        { // debug
            y[0]=max(1e-10,lambda);
            y[1]=Th;
            y[2]=Vh;        
            double Ph  = profile.P(y);
            double eh  = profile.HE->e(Vh,Th);
            //
            double Vp  = profile.HE->V2;
            EOS_VT *products = profile.products;
            double hp  = products->e(Vp,Th)+Ph*Vp;
            double Kp  = products->KT(Vp,Th);
            double Gp  = products->Gamma(Vp,Th);
            double Cvp = products->CV(Vp,Th);
            //
            double Vr  = profile.HE->V1;
            EOS_VT *reactants = profile.reactants;
            double hr  = reactants->e(Vr,Th)+Ph*Vr;
            double Kr  = reactants->KT(Vr,Th);
            double Gr  = reactants->Gamma(Vr,Th);
            double Cvr = reactants->CV(Vr,Th);
            double dPdlambda = (Vp-Vr)/(lambda*Vp/Kp+(1.-lambda)*Vr/Kr);
            double dedlambda = hp-hr
                 -(lambda*Gp*Cvp/Kp+(1.-lambda)*Gr*Cvr/Kr)*Th*dPdlambda;
            cout << "dP/dlambda, de/dlambda = " << dPdlambda
                 << " " << dedlambda << "\n";
            // finite difference
            double dlambda = 0.0001;
            y[0] += dlambda;
            if( y[0] >= 1.0 )
            {
                dlambda = -dlambda;
                y[0] = lambda + dlambda;
            }
            double Pp = profile.P(y);
            double ep  = profile.HE->e(Vh,Th);
            dPdlambda = (Pp-Ph)/dlambda;
            dedlambda = (ep-eh)/dlambda;
            cout << "dP/dlambda, de/dlambda = " << dPdlambda
                 << " " << dedlambda << "\n";
        }
        if( debug == 2 )
        {
            y[1]=Th;
            y[2]=Vh;        
            double Ph = profile.P(y);
            //double dP = Ph - (profile.P0 + profile.m2*(V0-Vh));
            double dP = (Ph+profile.m2*(Vh-V0)) - profile.P0;
            double eh = profile.HE->e(Vh,Th);
            //double de = eh - (profile.e0+ 0.5*(Ph+profile.P0)*(V0-Vh));
            double de = (eh+ 0.5*(Ph+profile.P0)*(Vh-V0)) - profile.e0;
            cout << "\t deh/eh=" << de/eh << " \tdPh/Ph=" << dP/Ph ;
            double yp1 = yp[1];
            profile.F(yp,y,zeta);
            cout << " \tat (Th,Vh) dVdz=" << yp[2] << " \tdTdz=" << yp[1] << "\n";
            yp[1]=yp1;
        }
        if( debug == 3 )
        {
            double Ph = profile.P0 + profile.m2*(V0-Vh);
            cout << "\t(V/Vh-1,T/Th-1)= " << V/Vh-1. << " " << T/Th-1.
                 << " P/Ph-1=" << P/Ph-1. << "\n";
            double AV[2], AT[2];
            profile.Anr(Vh,Th,AV,AT);
            //profile.Ah(Vh,Th,AV,AT);
            double e[2],eT[2];
            profile.Eigen(AV,AT,e,eT);
            double delTdelV = (T/Th-1.)/(V/Vh-1.);
            double dTdVh = profile.dTdVh(Vh,Th);
            cout << "\t (T/Th-1)/(V/Vh-1): " << delTdelV
                 << " d(T/Th)/d(V/Vh): " << (Vh/Th)*yp[1]/yp[2]
                 << "  h+R: "     << (Vh/Th)*yp[3]/yp[4]
                 << "  h: "       << (Vh/Th)*dTdVh
                 << " eigenfuncs:  " << eT[1] << " (" << e[1] << ")"
                 <<              " " << eT[0] << " (" << e[0] << ")"
                 << "\n";
        }
        if( last || lambda>1.-1.e-4 )
            break;
        if( yp[1] > 0. && T < Tdefl )
            last=1;
    }
    if( 1 )
    { // diagnostic
        double Vi = profile.HE->V1;
        double ei = profile.reactants->e(Vi,T);
        cout << "Vr = " << Vi << ", er = " << ei << "\n";
        Vi = profile.HE->V2;
        ei = profile.products->e(Vi,T);
        cout << "Vp = " << Vi << ", ep = " << ei << "\n";
    }
    delete defl;
    delete hug;
    deleteEOS(Products);
    deleteEOS(Reactants);
    deleteEOS(eos);
    return 0;
}
