#include <EOS.h>
#include "IdealGas.h"
#include "IdealGas_Riemann.h"

RiemannSolver_IdealGas::~RiemannSolver_IdealGas()
{
	// NULL
}

int RiemannSolver_IdealGas::Solve(const HydroState &state_l,
		const HydroState &state_r,  WaveState &left, WaveState &right)
{
	double g_l = gas_l.gamma;
	double g_r = gas_r.gamma;

	double V_l = state_l.V;
	double u_l = state_l.u;
	double P_l = (g_l-1)*state_l.e/V_l;
	double c_l = sqrt(g_l*P_l*V_l);
	
	double V_r = state_r.V;
	double u_r = state_r.u;
	double P_r = (g_r-1)*state_r.e/V_r;
	double c_r = sqrt(g_r*P_r*V_r);

	double Pmin = min(P_l, P_r);
	
	double dudP_l = -V_l/c_l;
	double dudP_r =  V_r/c_r;

	double dP;
	double du = u_r - u_l;

	double P = (P_r*dudP_r - P_l*dudP_l - du) / (dudP_r-dudP_l);	
	P = max(P, Pmin );
	
	double ul_p, ur_p;
	
	int status = -1;
	int N_iter = max_iter;
	while( N_iter-- )
	{
	// P is monotonically increasing after first iteration
		if( P < P_l)
		{
			double V = V_l*pow(P_l/P, 1/g_l);
			double c = sqrt(g_l*P*V);
			ul_p  = u_l + 2/(g_l-1)*(c_l - c);
			dudP_l = - V/c;
		}
		else
		{
			double M2 = 1 + 0.5*(g_l+1)/g_l*(P/P_l - 1);
			double M = sqrt(M2);
			dudP_l = -0.5*V_l/c_l*(1+1/M2)/M;
			ul_p = u_l - 2/(g_l+1)*(1-1/M2)*M*c_l;
		}
		
		if( P < P_r)
		{
			double V = V_r*pow(P_r/P, 1/g_r);
			double c = sqrt(g_r*P*V);
			ur_p  = u_r + 2/(g_r-1)*(c - c_r);
			dudP_r = V/c;
		}
		else
		{
			double M2 = 1 + 0.5*(g_r+1)/g_r*(P/P_r - 1);
			double M = sqrt(M2);
			dudP_r = 0.5*V_r/c_r*(1+1/M2)/M;
			ur_p = u_r + 2/(g_r+1)*(1-1/M2)*M*c_r;
		}
		
		du = ur_p - ul_p;
		dP = -du/(dudP_r - dudP_l);
		
		P += dP;
		if( abs(dP) < RelErr*P )
		{
			status = 0;
			break;
		}

		if( dP < 0 && P < 0.1 * Pmin )
		{
		// Note: can only occur on first iteration
			double c = c_l * pow( Pvac/P_l, 0.5*(1-1/g_l) );
			ul_p  = u_l + 2/(g_l-1)*(c_l - c);
			
			c = c_r * pow( Pvac/P_r, 0.5*(1-1/g_r) );
			ur_p  = u_r + 2/(g_r-1)*(c - c_r);

			du = ul_p - ur_p;
			if( du < 0 )
			{	// vacuum state
				P = Pvac;
				status = 0;
				break;
			}
			double L = 2/(g_l -1) * c_l;
			double R = 2/(g_r -1) * c_r;
			
			double n_l = 0.5*(g_l-1)/g_l;
			double n_r = 0.5*(g_r-1)/g_r;
			double n = min(n_l, n_r);
			
			du /= (L*pow(Pmin/P_l, n_l) + R*pow(Pmin/P_r, n_r));
			P = Pmin*pow(du, 1/n);
		}
	}
	
	if( status )
		return status;	// error
	
	Wave *wave;
	if( P < P_l )
		wave = gas_l.isentrope(state_l);
	else
		wave = gas_l.shock(state_l);
	
	wave->P(P,LEFT, left);
	delete wave;
	
	if( P < P_r )
		wave = gas_r.isentrope(state_r);
	else
		wave = gas_r.shock(state_r);
	wave->P(P,RIGHT,right);
	delete wave;
	
	return 0;
}

