#include "ExtEOS.h"
#include "Riemann_genX.h"

RiemannSolver_genX::RiemannSolver_genX(ExtEOS &left,  const double *zleft,
                                       ExtEOS &right, const double *zright)
{
    EOS *xleft  =  left.CopyEOS(zleft);
    EOS *xright = right.CopyEOS(zright);
    if( xleft == NULL || xright == NULL )
    {
        EOSbase::ErrorDefault->Log("RiemannSolver_genX::RiemannSolver_genX",
                      __FILE__, __LINE__, NULL, "Copy failed\n");
	    RS = NULL;
        return;
    }
    RS = new RiemannSolver_generic(*xleft, *xright);
    deleteEOS(xleft);
    deleteEOS(xright);
}
