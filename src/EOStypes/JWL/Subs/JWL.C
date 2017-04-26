#include "JWL.h"
#include "JWL_VT.h"

JWL::JWL() : EOS("JWL")
{
    V0 = NaN;
    e0 =0.0;
    A  = R1 = NaN;
    B  = R2 = NaN;
    C  = omega = NaN;
    de = Edet = NaN;
    Cv = Scj = Tcj = Vcj = Pcj = NaN;
}

JWL::~JWL()
{
	// Null
}

#define FUNC "JWL::JWL(JWL_VT &eos)",__FILE__,__LINE__,this
JWL::JWL(JWL_VT &eos) : EOS("JWL")
{
    Transfer(eos);
    V0    = eos.V0;
    e0    = eos.e0;
    A     = eos.A;
    R1    = eos.R1;
    B     = eos.B;
    R2    = eos.R2;
    C     = eos.C;
    omega = eos.omega;
    Edet  = eos.Edet;
    de    = eos.de;
    Cv    = eos.Cv;
    Tcj   = eos.Tcj;
    Scj   = eos.Scj;
    Vcj   = eos.Vcj;
    Pcj   = eos.Pcj;
    if( std::isnan(C) )
    {
        if( Edet <= 0.0 )
        { // replace (Vcj,ecj) with (V_0,e0) for ref isentrope
            C  = Pcj-A*exp(-R1)-B*exp(-R2);
	        Vcj = V0;
	        if( std::isnan(e_ref) )
	            e_ref = V0*((A/R1)*exp(-R1)+(B/R2)*exp(-R2)+C/omega) -de;       
	        if( std::isnan(Tcj) )
	            Tcj = (V0/omega)*(Pcj-A*exp(-R1)-B*exp(-R2))/Cv;
	        if( std::isnan(Scj) )
	            Scj = 0.;            
        }
        else if( de > 0. )
        {
            HydroState state0;
            state0.V = V0;
            state0.e = e0;
            WaveState CJ;
            double P0 = 0.0;
            Detonation *det = detonation(state0,P0);
            if( det == NULL || det->CJwave(RIGHT,CJ) )
            {
                EOSerror->Log(FUNC,"failed to find CJ state\n");
            }
            else
            {
                Vcj = CJ.V;
                Pcj = CJ.P;
                delete det;
                C = omega*pow(Vcj/V0,omega)*( (CJ.e+de)/V0
                           -(A/R1)*exp(-R1*Vcj/V0)-(B/R2)*exp(-R2*Vcj/V0) );
                if( std::isnan(Tcj) )
                    Tcj = ( CJ.e+de - V0*
                              (A/R1*exp(-R1*Vcj/V0)+B/R2*exp(-R2*Vcj/V0)) )/Cv;
                if( std::isnan(Scj) )
                {
                    double T0 = T(V0,e0);
                    Scj = Edet/T0 - Cv*(log(T0/Tcj)+omega*log(V0/Vcj));
                }
            }
        }
    }
    EOSstatus = eos.status();
}

EOS_VT *JWL::VT()
{
    return new JWL_VT(*this);
}
