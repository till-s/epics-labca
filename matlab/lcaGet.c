#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
double *pres = 0;
int     i,n = 0;
mxArray *tmp;
PVs     pvs;
TimeArg re=0,im=0;
int     hasImag=0;

	if ( nrhs != 1 ) {
		MEXERRPRINTF("Expected one rhs argument");
		return;
	}

	if ( nlhs > 2 ) {
		MEXERRPRINTF("Too many output args");
		return;
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

    multi_ezca_get( pvs.names, -1, (void**)&pres, pvs.m, &n, &re, &im, &hasImag );

	if ( pres ) {
		if ( !(plhs[0] = mxCreateDoubleMatrix(pvs.m,n,mxREAL)) ) {
			MEXERRPRINTF("Not enough memory");
			goto cleanup;
		}
		memcpy(mxGetPr(plhs[0]), pres, sizeof(double) * pvs.m * n);
		if ( nlhs > 1 ) {
			/* give them the time stamps */
			if ( !(plhs[1] = mxCreateDoubleMatrix(pvs.m,1,mxCOMPLEX)) ) {
				MEXERRPRINTF("Not enough memory");
				goto cleanup;
			}
			/* these actually release the timestamps in an obscure way */
			cts_stampf_( &pvs.m, &re, mxGetPr(plhs[1]) );
			cts_stampf_( &pvs.m, &im, mxGetPi(plhs[1]) );
		} else {
			plhs[1] = 0;
		}
	}

cleanup:
	mxFree(pres);
	timeArgsRelease( &re, &im, &hasImag );
	releasePVs(&pvs);
}
