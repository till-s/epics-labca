#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
void	*pres = 0;
int     i,n = 0;
const mxArray *tmp;
mxArray *clean0 = 0, *clean1 = 0;
PVs     pvs = { 0 };
TimeArg re=0,im=0;
int		hasImag=0;
char	type = ezcaNative;
char	typestr[2] = { 'N', 0 };

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	if ( nlhs > 2 ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}

	if ( nrhs < 1 || nrhs > 3 ) {
		MEXERRPRINTF("Expected 1..3 rhs argument");
		goto cleanup;
	}

	/* check for an optional 'column dimension' argument */
	if ( nrhs > 1 ) {
		if ( ! mxIsNumeric(tmp = prhs[1]) || 1 != mxGetM(tmp) || 1 != mxGetN(tmp) ) {
			MEXERRPRINTF("2nd argument must be a numeric scalar");
			goto cleanup;
		}
		n = (int)mxGetScalar( tmp );
	}

	/* check for an optional data type argument */
	if ( nrhs > 2 ) {
		if ( ezcaInvalid == (type = marg2ezcaType( prhs[2] )) ) {
			goto cleanup;
		}
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

    multi_ezca_get( pvs.names, type, &pres, pvs.m, &n, &re, &im, &hasImag );

	/* if pres != NULL, we have a valid reply... */
	if ( pres ) {
		if ( ezcaString == type ) {
			/* convert string array to a matlab cell array of matlab strings */
			if ( !(clean0 = plhs[0] = mxCreateCellMatrix(pvs.m, n)) ) {
				MEXERRPRINTF("Not enough memory");
				goto cleanup;
			}
			for ( i = 0; i < pvs.m * n; i++ ) {
				if ( !(tmp = mxCreateString(((const char**)pres)[i])) ) {
					MEXERRPRINTF("Not enough memory");
					goto cleanup;
				}
				mxSetCell(plhs[0], i, (mxArray*)tmp);
			}
		} else {
			/* create a double matrix and copy (we don't know the dimension
			 * prior to calling multi_ezca_get() - otherwise we could directly
			 * pass the mxGetPr() handle to multi_ezca_get()...
			 */

			if ( !(clean0 = plhs[0] = mxCreateDoubleMatrix(pvs.m,n,mxREAL)) ) {
				MEXERRPRINTF("Not enough memory");
				goto cleanup;
			}
			memcpy(mxGetPr(plhs[0]), pres, sizeof(double) * pvs.m * n);
		}

		/* If requested, generate the timestamp matrix */
		if ( nlhs > 1 ) {
			/* give them the time stamps */
			if ( !(clean1 = plhs[1] = mxCreateDoubleMatrix(pvs.m,1,mxCOMPLEX)) ) {
				MEXERRPRINTF("Not enough memory");
				goto cleanup;
			}
			/* these actually release the timestamps in an obscure way */
			cts_stampf_( &pvs.m, &re, mxGetPr(plhs[1]) );
			cts_stampf_( &pvs.m, &im, mxGetPi(plhs[1]) );
		}
	}
	clean0 = clean1 = 0;

cleanup:
	if ( clean0 ) {
		mxDestroyArray( clean0 );
		plhs[0] = 0;
	}
	if ( clean1 ) {
		mxDestroyArray( clean1 );
		plhs[1] = 0;
	}
	if ( ezcaString == type && pres ) {
		/* free string elements also */
		for ( i=0; i<pvs.m*n; i++ ) {
			mxFree( ((char**)pres)[i] );
		}
	}
	mxFree(pres);
	timeArgsRelease( &re, &im, &hasImag );
	releasePVs(&pvs);
}
