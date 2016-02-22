#include <Arg.h>
#include "HEburn.h"
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.sam.FF";
    const char *units = NULL;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    //
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    //
    HErate *rate = HE->rate;
    int i,j;
    for(i=0; i<100; i++)
    {
        cerr << "i=" << i << "\n";
        for(j=0; j<20; j++ )
        {
            //HErate *rate1 = rate->Copy(HE);
            //delete rate1;
            //ExtEOS *xeos = HE->Copy();
            //deleteExtEOS(xeos);
            double z[1] = {0.5};
            EOS *eos = HE->CopyEOS(z);
            deleteEOS(eos);
        }
    }
    //
    deleteEOS(eos);
    return 0;
}
