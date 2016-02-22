#include <Arg.h>

#include <EOS.h>
#include <EOS_VT.h>
#include <Sesame.h>
#include <SesSubs.h>
#include <fstream>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "Sesame";
	const char *name = "PBX9501";
	const char *units = NULL;
    const char *plot = "test3.dg";

    EOS::Init();
    double V = 2.5459267100167775;
    double e = 1.2062497505345786;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(e,e);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	EOS *eos = FetchEOS(type,name,db);
	if( eos == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    Sesame *seos = dynamic_cast<Sesame*>(eos);
    if( seos == NULL )
		cerr << Error("seos is NULL" ) << Exit;
    Sesame_VT *sesVT = seos->eosVT();
    if( sesVT == NULL )
		cerr << Error("sesVT is NULL" ) << Exit;
    //
    double T = eos->T(V,e);
    cout << "T = " << T << "\n";
    //
    double P  = eos->P(V,e);
    double P1 = sesVT->P(V,T);
    cout << "P = " << P << "," << P1 << "\n";
    //
    double KS  = eos->KS(V,e);
    double KS1 = sesVT->KS(V,T);
    cout << "KS = " << KS << ", " << KS1 << "\n";
    //
    double KT  = eos->KT(V,e);
    double KT1 = sesVT->KT(V,T);
    cout << "KT = " << KT << ", " << KT1 << "\n";
    //
    
    double rho = 1/V;
    int i = SESsrch(rho, sesVT->rho_grd,sesVT->rho_n);
    int j = SESsrch(T, sesVT->T_grd, sesVT->T_n);
    cout << i << " " << sesVT->rho_grd[i] << " " << sesVT->rho_grd[i+1] << "\n";
    cout << j << " " << sesVT->T_grd[j] << " " << sesVT->T_grd[j+1] << "\n";

    double rho_min = 0.1;
    double rho_max = 1.0;
    double P_min = 0.;
    double P_max = 0.4;
    ofstream Plot(plot);
    Plot << ".view 0,0,1,1;\n"
         << ".pointsize 18;\n"
         << ".penwidth 2.5;\n"
         << ".ticlength 0.8;\n"
         << ".limits x[" << rho_min << "," << rho_max << "]\n"
         << ".limits y[" <<   P_min << "," <<   P_max << "]\n"
         << ".clip 0 0 1 1\n"
         << "\n";
    int jmin = max(0,j-1);
    int jmax = j+2;
    int jj;
    for( jj=jmin; jj<=jmax; jj++ )
    {
       int iz = sesVT->rho_n*jj;
       int k;
       Plot << ".line\n";
       for(k=0; k<sesVT->rho_n; k++)
          Plot << sesVT->rho_grd[k] << " " << sesVT->P_table[iz+k] << "\n";
    }
    Plot << "\n"
         << ".noclip\n"
         << ".black\n"
         << ".line symbol solid\n"
         << ".pointsize 18;\n"
         << ".penwidth 2.5;\n"
         << "\n";
    

    

/******************************    
    double V1 = 1./0.40;
    double V2 = 1./0.35;
    double Ti[2] = {1000.,1250.};
    double Pi[2];
    double Pi_V[2];
    double Vi;
    int i;
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    for( i=0; i<=10; i++ )
    {
        Vi = V1 + double(i)/10. *(V2-V1);
        int k;
        for( k=0; k<=1; k++ )
        {
            Pi[k] = sesVT->P(Vi,Ti[k]);
            Pi_V[k] = sesVT->P_V(Vi,Ti[k]);
        }
        cout << setw(13) << setprecision(6) << Vi  << " "
             << setw(13) << setprecision(6) <<Ti[0] << " "
             << setw(13) << setprecision(6) <<Pi[0] << " "
             << setw(13) << setprecision(6) <<Pi_V[0] << "\n";
        cout << setw(28) << " "
             << setw(13) << setprecision(6) <<Pi[1] << " "
             << setw(13) << setprecision(6) <<Pi_V[1] << "\n";
    }
*************************************/    

    
    //
    EOS_VT *VTeos = static_cast<EOS_VT*>(sesVT);
    deleteEOS_VT(VTeos);
    deleteEOS(eos);
    return 0;
}
