#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
char	**pstr = 0;
int     i, m = 0, n = 0;
const	mxArray *tmp, *strval;
PVs     pvs = { 0 };
char	type = ezcaNative;
mxArray *dummy = 0;

	if ( nlhs ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}

	if ( nrhs < 2 || nrhs > 3 ) {
		MEXERRPRINTF("Expected 2..3 rhs argument");
		goto cleanup;
	}

	n = mxGetN( tmp = prhs[1] );
    m = mxGetM( tmp );

	if ( mxIsChar( tmp ) ) {
		/* a single string; create a dummy cell matrix */
		m = n = 1;
		if ( !(dummy = mxCreateCellMatrix( m, n )) ) {
			MEXERRPRINTF("Not enough memory");
			goto cleanup;
		}
		mxSetCell(dummy, 0, (mxArray*)tmp);
		tmp = dummy;
	}

	if ( mxIsCell( tmp ) ) {
		if ( !(pstr = mxCalloc( m * n, sizeof(*pstr) )) ) {
			MEXERRPRINTF("Not enough memory");
			goto cleanup;
		}
		for ( i = 0; i < m * n; i++ ) {
			int len;
			if ( !mxIsChar( (strval = mxGetCell(tmp, i)) ) || 1 != mxGetM( strval ) ) {
				MEXERRPRINTF("Value argument must be a cell matrix of strings");
				goto cleanup;
			}
			len = mxGetN(strval) * sizeof(mxChar) + 1;
			if ( !(pstr[i] = mxMalloc(len)) ) {
				MEXERRPRINTF("Not enough memory");
				goto cleanup;
			}
			if ( mxGetString(strval, pstr[i], len) ) {
				MEXERRPRINTF("Value still not a string (after all those checks) ???");
				goto cleanup;
			}
		}
		type = ezcaString;
	} else if ( ! mxIsDouble(tmp) ) {
			MEXERRPRINTF("2nd argument must be a double matrix");
			goto cleanup;
	} else {
		/* is a DOUBLE matrix */
	}

	if ( nrhs > 2 ) {
		char tmptype;
		if ( ezcaInvalid == (tmptype = marg2ezcaType(prhs[2])) ) {
			goto cleanup;
		}
		if ( ezcaString == type && ezcaString != tmptype ) {
			MEXERRPRINTF("string value type conversion not implemented, sorry");
			goto cleanup;
		}
		type = tmptype;
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

	assert( (pstr != 0) == (ezcaString ==  type) );

	multi_ezca_put( pvs.names, pvs.m, type, (pstr ? (void*)pstr : (void*)mxGetPr(prhs[1])), m, n);

cleanup:
	if ( pstr ) {
		/* free string elements also */
		for ( i=0; i<m*n; i++ ) {
			mxFree( pstr[i] );
		}
		mxFree( pstr );
	}
	if ( dummy ) {
		mxSetCell( dummy, 0, 0 );
		mxDestroyArray( dummy );
	}
	releasePVs(&pvs);
}