/* $Id: mglue.c,v 1.15 2004/03/27 00:22:30 till Exp $ */

/* MATLAB - EZCA interface glue utilites */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <cadef.h>
#include <ezca.h>

#define epicsExportSharedSymbols
#include "shareLib.h"
#include "multiEzca.h"
#include "mglue.h"

void epicsShareAPI
releasePVs(PVs *pvs)
{
int i;
	if ( pvs && pvs->names ) {
		for ( i=0; i<pvs->m; i++ ) {
			mxFree(pvs->names[i]);
		}
		mxFree( pvs->names );
		multi_ezca_ctrlC_epilogue( &pvs->ctrlc );	
	}
}

int epicsShareAPI
buildPVs(const mxArray *pin, PVs *pvs)
{
char	**mem = 0;
int     i,m,buflen;
const mxArray *tmp;
int	rval = -1;

	if ( !pvs )
		return -1;

	pvs->names = 0;
	pvs->m     = 0;

	if ( !pin )
		return -1;

	if ( mxIsCell(pin) &&  1 != mxGetN(pin) ) {
			MEXERRPRINTF("Need a column vector of PV names\n");
			goto cleanup;
	}

	m = mxIsCell(pin) ? mxGetM(pin) : 1;

	if ( (! mxIsCell(pin) && ! mxIsChar(pin)) || m < 1 ) {
		MEXERRPRINTF("Need a cell array argument with PV names");
		/* GENERAL CLEANUP NOTE: as far as I understand, this is not necessary:
		 *                       in mex files, garbage is automatically collected;
		 *                       explicit cleanup works in standalong apps also, however.
		 */
		goto cleanup;
	}

	if ( ! (mem = mxCalloc(m, sizeof(*mem))) ) {
		MEXERRPRINTF("No Memory\n");
		goto cleanup;
	}


	for ( i=buflen=0; i<m; i++ ) {
		tmp = mxIsCell(pin) ? mxGetCell(pin, i) : pin;		
		if ( !tmp || !mxIsChar(tmp) || 1 != mxGetM(tmp) ) {
			MEXERRPRINTF("Not an vector of strings??");
			goto cleanup;
		}
		buflen = mxGetN(tmp) * sizeof(mxChar) + 1;
		if ( !(mem[i] = mxMalloc(buflen)) ) {
			MEXERRPRINTF("No Memory\n");
			goto cleanup;
		}
		if ( mxGetString(tmp, mem[i], buflen) ) {
			MEXERRPRINTF("not a PV name?");
			goto cleanup;
		}
	}

	pvs->names = mem;
	pvs->m     = m;
	rval       = 0;

	multi_ezca_ctrlC_prologue(&pvs->ctrlc);

	pvs        = 0;

cleanup:
	releasePVs(pvs);
	return rval;	
}

char epicsShareAPI
marg2ezcaType(const mxArray *typearg)
{
char typestr[2] = { 0 };

	if ( ! mxIsChar(typearg) ) {
		MEXERRPRINTF("(optional) type argument must be a string");
	} else {
		mxGetString( typearg, typestr, sizeof(typestr) );
		switch ( toupper(typestr[0]) ) {
			default:
				MEXERRPRINTF("argument specifies an invalid data type");
 				return ezcaInvalid;

			case 'N':	return ezcaNative;
			case 'B':	return ezcaByte;
			case 'S':	return ezcaShort;
			case 'I':
			case 'L':	return ezcaLong;
			case 'F':	return ezcaFloat;
			case 'D':	return ezcaDouble;
			case 'C':	return ezcaString;
		}
	}
	return ezcaInvalid;
}

void epicsShareAPI
flagError(int nlhs, mxArray *plhs[])
{
int i;
	if ( nlhs ) {
		for ( i=0; i<nlhs; i++ ) {
			mxDestroyArray(plhs[i]);
			/* hope this doesn't fail... */
			plhs[i] = 0;
		}
		mexErrMsgTxt("(see above error messages)");
	}
}

void epicsShareAPI
theLcaPutMexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], int doWait)
{
char	**pstr = 0;
int     i, m = 0, n = 0;
int		rval;
const	mxArray *tmp, *strval;
PVs     pvs = { {0}, };
char	type = ezcaNative;
mxArray *dummy = 0;
	
#ifdef LCAPUT_RETURNS_VALUE
	if ( nlhs == 0 )
		nlhs = 1;

	if ( nlhs > 1 ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}
#else
	if ( nlhs ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}
	nlhs = -1;
#endif

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
		if ( (ezcaString == type) != (ezcaString == tmptype) ) {
			MEXERRPRINTF("string value type conversion not implemented, sorry");
			goto cleanup;
		}
		type = tmptype;
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

	assert( (pstr != 0) == (ezcaString ==  type) );

	rval = multi_ezca_put( pvs.names, pvs.m, type, (pstr ? (void*)pstr : (void*)mxGetPr(prhs[1])), m, n, doWait);

	if ( rval > 0 ) {
#ifdef LCAPUT_RETURNS_VALUE
		if ( !(plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL)) ) {
			MEXERRPRINTF("Not enough memory");
			goto cleanup;
		}
		*mxGetPr(plhs[0]) = (double)rval;
#endif
		nlhs = 0;
	}

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
	/* do this LAST (in case the use mexErrMsgTxt) */
	flagError(nlhs, plhs);
}
