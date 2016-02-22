#include <Arg.h>
#include <LocalMath.h>
#include <EOS_VT.h>
#include <Sesame_VT.h>
#include <OneDFunction.h>

class P_eosVT : private OneDFunction
{
private:
    EOS_VT *eos;
    double V;
public:
    P_eosVT(EOS_VT *e, double v) : eos(e), V(v) {}
    ~P_eosVT() {}
    double f(double T); // OneDFunction::f

    int findT(double p, double &T); // T at (V,p)
};

double P_eosVT::f(double T)
{
    return eos->P(V,T);
}
int P_eosVT::findT(double p, double &T)
{
    double T1 = eos->T_ref;
    double P1 = eos->P(V,T1);
    if( isnan(P1) )
        return -1;
    double T2, P2;
    if( p >P1)
    {
        int i;
        for( i=1; i<21; i++ )
        {
            double dPdT = eos->P_T(V,T1);
            if( dPdT <= 0.0 )
                return -1;
            T2 = T1 + 2.*(p-P1)/dPdT;
            P2 = eos->P(V,T2);
            if( isnan(P2) )
                return -1;
            if( P2 == p )
            {
                T = T2;
                return 0;
            }
            else if( P2 > p )
                break;
            T1 = T2;
            P1 = P2;
        }   
    }
    else if( p < P1 )
    {
        int i;
        for( i=1; i<21; i++ )
        {
            double dPdT = eos->P_T(V,T1);
            if( dPdT <= 0.0 )
                return -1;
            T2 = T1 + 2.*(p-P1)/dPdT;
            if( T2 <= 0. )
                T2 = 0.5*T1;
            P2 = eos->P(V,T2);
            if( isnan(P2) )
                return -1;
            if( P2 == p )
            {
                T = T2;
                return 0;
            }
            else if( P2 < p )
                break;
            T1 = T2;
            P1 = P2;
        }   
    }
    else
    {
        T = T1;
        return 0;
    }
    T = inverse(p, T1,P1, T2,P2);
    int status = Status();
    if( status )
    {
        eos->ErrorHandler()->Log("e_EOS::e", __FILE__, __LINE__, eos,
                    "inverse failed with status: %s\n", ErrorStatus() );
        return status;
    }
    return 0;
}


int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = NULL;
	const char *type = "Sesame";
	const char *name = NULL;
	const char *units = NULL;

    double V = 0.0;
    double rho = 0.0;
    double P = 0.0;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(rho,rho);
		GetVar(P,P);
		
		ArgError;
	}
	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;
	if( name == NULL )
		cerr << Error("Must specify Sesame material name") << Exit;
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	EOS_VT *eos = FetchEOS_VT(type,name,db);
	if( eos == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    // eos->Print(cout) << Exit;
    if( rho > 0.0 )
        V = 1./rho;
    if( P <= 0.0 || V <= 0.0 )
		cerr << Error("Must specify V and P" ) << Exit;
    P_eosVT PVT(eos,V);
    double T;
    if( PVT.findT(P,T) )
		cerr << Error("findT failed" ) << Exit;
    cout << "T= " << T << "  at V,P=  " << V << "," << P << "\n";
    double e = eos->e(V,T);
    double c2 = eos->c2(V,T);
    double c = (c2 > 0.) ? sqrt(c2) : 0.0;
    cout << "e= " << e << "  c= " << c << "\n";
    deleteEOS_VT(eos);
    return 0;
}
