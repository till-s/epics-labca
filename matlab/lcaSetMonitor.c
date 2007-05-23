/* $Id: lcaSetMonitor.c,v 1.4 2007-05-21 22:02:01 till Exp $ */

/* matlab wrapper for ezcaSetMonitor */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int     i,n = 0;
const mxArray *tmp;
PVs     pvs = { {0} };
char	type = ezcaNative;
LcaError theErr;

	lcaErrorInit(&theErr);

	if ( nlhs == 0 )
		nlhs = 1;

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	if ( nlhs > 1 ) {
		lcaRecordError(EZCA_INVALIDARG, "Too many output args", &theErr);
		goto cleanup;
	}

	if ( nrhs < 1 || nrhs > 3 ) {
		lcaRecordError(EZCA_INVALIDARG, "Expected 1..3 rhs argument", &theErr);
		goto cleanup;
	}

	/* check for an optional 'column dimension' argument */
	if ( nrhs > 1 ) {
		if ( ! mxIsNumeric(tmp = prhs[1]) || 1 != mxGetM(tmp) || 1 != mxGetN(tmp) ) {
			lcaRecordError(EZCA_INVALIDARG, "2nd argument must be a numeric scalar", &theErr);
			goto cleanup;
		}
		n = (int)mxGetScalar( tmp );
	}

	/* check for an optional data type argument */
	if ( nrhs > 2 ) {
		if ( ezcaInvalid == (type = marg2ezcaType( prhs[2], &theErr )) ) {
			goto cleanup;
		}
	}

	if ( buildPVs(prhs[0], &pvs, &theErr) )
		goto cleanup;

    if ( 0 == multi_ezca_set_mon( pvs.names, pvs.m, type, n, &theErr ) ) {
		nlhs = 0;
	}

cleanup:
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
