#include <LocalIo.h>
#include <Arg.h>

#include "ExtEOSmanager.h"
#include "Hydro.h"

int main(int, char **argv)
{
	ProgName(*argv);
	//const char *file = "../../DATA/EOS.data";
	std::string file_;
        file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
	file_ += "/EOS.data";
        const char * file = file_.c_str();
	const char *uname = "std";
	//const char *lib = "../lib/Linux";
	//const char *lib = "../../lib";
	std::string libPath;
        libPath  = (getenv("EOSLIB_SHARED_LIBRARY_PATH") != NULL) ? getenv("EOSLIB_SHARED_LIBRARY_PATH") : "PATH ENV NOT SET!";
        const char * lib     = libPath.c_str();
    
	const char *mat[] = {
        "EqExtEOS::HMX",
        "Hayes::HMX",
        "BirchMurnaghan::HMX",
        "JWL::PBX9501",
        "JWL::PBX9404",
        "Hayes::HMX",
         NULL };


	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
        GetVar(uname,uname);
        GetVar(lib,lib);
		ArgError;
	}

    ExtEOSmanager materials(cout);
    char *SharedLibDir = Cat( "SharedLibDirEOS=",lib );
    materials.SetEnv(SharedLibDir);
    delete [] SharedLibDir;
    if( materials.Read(file) )
        cerr << Error("error reading file ") << file << Exit;

    int i, n;
    for( n=0; mat[n]; n++ )
    {
        i = materials.index_eos(mat[n],uname);
        cout << "EOS " << i << " -> " << mat[n] << "\n";
    }
    cout << "\n\n";

    for( i=0; i<n; i++ )
    {
        if( !materials.valid_eos(i) )
            continue;
        ExtEOS &xeos = materials.xeos(i);
        xeos.Print(cout << "ExtEOX.Print\n");
        if( !strcmp("Hydro",xeos.Type()) )
        {
            Hydro &hydro = dynamic_cast<Hydro&>(xeos);
            hydro.PrintHydro(cout << "PrintHydro\n");
        }
        
        //EOS &eos = materials.eos(i);
        EOS &eos = static_cast<EOS &>(xeos);
        char *type_name = Cat( eos.Type(), "::", eos.Name() );
        ExtEOSstate *state = new ExtEOSstate(xeos );
        int j = materials.AddState( type_name, state );
        cout << "state " << j << "\n"
             << materials.state(j) << "\n";
        delete [] type_name;
    }
    return 0;
}
