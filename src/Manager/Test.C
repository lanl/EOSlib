#include <Arg.h>

#include "EOSmanager.h"

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "../../DATA/EOS.data";
    const char *uname = "std";
    const char *lib = "../lib/Linux";

    const char *mat[] = {
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

    EOSmanager materials(cout);
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


    for( i=0; i<n; i++ )
    {
        if( !materials.valid_eos(i) )
            continue;
        EOS &eos = materials.eos(i);
        char *type_name = Cat( eos.Type(), "::", eos.Name() );
        EOSstate *state = new EOSstate(eos, eos.V_ref, eos.e_ref );
        int j = materials.AddState( type_name, state );
        cout << "state " << j << "\n"
             << materials.state(j) << "\n";
        delete [] type_name;
    }

    i = materials.index_units(uname);
    if( i < 0 )
        cerr << Error("no units ") << uname << Exit;
    cout << "units " << i << " -> " << uname << "\n";

    return 0;
}
