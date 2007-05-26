/* $Id: lcaLastError.c,v 1.1 2007/05/24 19:35:24 till Exp $ */

/* matlab wrapper for lcaGetLastError */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2007 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

#include <stdint.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int      i;
int      *src;
int32_t  *dst;
LcaError *ptheErr = lcaGetLastError();
LcaError theErr;

	if ( nlhs == 0 )
		nlhs = 1;

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	lcaErrorInit(&theErr);

	if ( nlhs > 1 ) {
		lcaRecordError(EZCA_INVALIDARG, "Too many output args", &theErr);
		goto cleanup;
	}

	if ( nrhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Expected no rhs argument", &theErr);
		goto cleanup;
	}

	if ( (i=ptheErr->nerrs) ) {
		src = ptheErr->errs;
	} else {
		i   = 1;
		src = &ptheErr->err;
	}

	if ( ! (plhs[0] = mxCreateNumericMatrix( i, 1, mxINT32_CLASS, mxREAL )) ) {
		lcaRecordError(EZCA_FAILEDMALLOC, "Not enough memory", &theErr);
		goto cleanup;
	}

	dst = mxGetData(plhs[0]);

	while ( i-- ) {
		*dst++ = *src++;
	}

	nlhs = 0;

cleanup:

	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
