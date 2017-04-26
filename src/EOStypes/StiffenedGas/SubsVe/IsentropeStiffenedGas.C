#include <EOS.h>
#include "StiffenedGas.h"


class Isentrope_StiffenedGas : public Isentrope
{
private:
	Isentrope_StiffenedGas(EOS &Gas);			// disallowed
	void operator=(const Isentrope_StiffenedGas&);		// disallowed
	Isentrope_StiffenedGas(const Isentrope_StiffenedGas&);	// disallowed
	Isentrope_StiffenedGas *operator &();			// disallowed
	
	StiffenedGas &gas;
public:
	Isentrope_StiffenedGas(StiffenedGas &Gas, double p = EOS::NaN)
                        : Isentrope(Gas,p), gas(Gas) {}
	~Isentrope_StiffenedGas();
	
	// find state on isentrope at specified V, P or u
	int V(double V, int direction, WaveState &wave);
	int P(double p, int direction, WaveState &wave);
	int u(double u, int direction, WaveState &wave);
	int u_s(double us, int direction, WaveState &wave);
	
	double u_escape(int direction);
	int u_escape(int direction, WaveState &wave);
};



Isentrope_StiffenedGas::~Isentrope_StiffenedGas()
{
	// NULL
}

int Isentrope_StiffenedGas::V(double V, int dir, WaveState &wave)
{
    if( V <= 0 )
        return -1;

    double P_p = gas.P_p;
	double e_p = gas.e_p;
	double G   = gas.G;
	
	wave.V  = V;
	double p = (state0.P+P_p)*pow(state0.V/V, G+1);
	if( (wave.P=p-P_p) < p_vac )
        return P(p_vac,dir,wave);  
	wave.e  = (p/G+P_p)*V+e_p;
	double c = sqrt((G+1)*p*V);
	wave.u  = state0.u - dir * 2/G *(state0.c - c);
	wave.us = wave.u + dir * c;	// trailing edge
	return 0;
}

int Isentrope_StiffenedGas::P(double p, int dir, WaveState &wave)
{
    int status = (p<p_vac) ? 1 : 0;
    p = max(p,p_vac);
    
	double P_p = gas.P_p;
	double e_p = gas.e_p;
	double G   = gas.G;

	wave.P = p;
	p += P_p;
	wave.V = state0.V * pow((state0.P+P_p)/p, 1/(G+1));
	wave.e  = (p/G+P_p)*wave.V+e_p;
	double c = sqrt((G+1)*p*wave.V);
	wave.u  = state0.u - dir * 2/G *(state0.c - c);
	wave.us = wave.u + dir * c;	// trailing edge
	return status;	
}

int Isentrope_StiffenedGas::u(double u, int dir, WaveState &wave)
{
	double P_p = gas.P_p;
	double e_p = gas.e_p;
	double G   = gas.G;

	double c = state0.c + dir *0.5*G*(u -state0.u);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow((G+1)*state0.V*(state0.P+P_p)/(c*c),1/G);
	double p = c*c/((G+1)*wave.V);
	if( (wave.P=p-P_p) < p_vac )
        return P(p_vac,dir,wave);        
	wave.e  = (p/G+P_p)*wave.V+e_p;
	wave.u = u;
	wave.us = wave.u + dir * c;	// trailing edge
	return 0;	
}

int Isentrope_StiffenedGas::u_s(double us, int dir, WaveState &wave)
{
	double P_p = gas.P_p;
	double e_p = gas.e_p;
	double G   = gas.G;

	double c = (2*state0.c + dir*G*(us -state0.u))/(G+2);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow((G+1)*state0.V*(state0.P+P_p)/(c*c),1/G);
	double p = c*c/((G+1)*wave.V);
	if( (wave.P=p-P_p) < p_vac )
        return P(p_vac,dir,wave);        
	wave.e  = (p/G+P_p)*wave.V+e_p;
	wave.us = us;			// trailing edge
	wave.u  = us -dir*c;
	return 0;	
}

double Isentrope_StiffenedGas::u_escape(int dir)
{
	// c > 0 at P = p_vac, not true escape velocity
	double P_p = gas.P_p;
	double G   = gas.G;

	double V = state0.V * pow((P_p+state0.P)/(P_p+p_vac), 1/(G+1));
	double c = sqrt((G+1)*(P_p+p_vac)*V);
	return state0.u - dir * 2/G *(state0.c - c);
}

int Isentrope_StiffenedGas::u_escape(int direction, WaveState &wave)
{
     return P(p_vac, direction, wave);   
}

Isentrope *StiffenedGas::isentrope(const HydroState &state)
{
	Isentrope *S = new Isentrope_StiffenedGas(*this);
	int status = S->Initialize(state);
	
	if( status )
	{
	    EOSerror->Log("StiffenedGas::isentrope",
			   __FILE__, __LINE__, this,
	                   "Initialize failed with status %d\n", status);
	    delete S;
	    S = NULL;
	}

	return S;
}


