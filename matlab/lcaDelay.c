/* $Id: lcaDelay.c,v 1.5 2007-05-24 19:35:24 till Exp $ */

/* matlab wrapper for ezcaDelay */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;

	lcaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);

	if ( 1 < nlhs ) {
		lcaSetError(&theErr, EZCA_INVALIDARG, "Need one output arg");
		goto cleanup;
	}

	if ( 1 != nrhs ) {
		lcaSetError(&theErr, EZCA_INVALIDARG, "Expected one rhs argument");
		goto cleanup;
	}

	if ( !mxIsDouble(prhs[0]) || 1 != mxGetM(prhs[0]) || 1 != mxGetN(prhs[0]) ) {
		lcaSetError(&theErr, EZCA_INVALIDARG, "Need a single numeric argument");
		goto cleanup;
	}

	if ( !ezcaDelay((float)*mxGetPr(prhs[0])) ) {
		nlhs = 0;
	} else {
		lcaSetError(&theErr, EZCA_INVALIDARG, "Need a timeout argument > 0.0\n");
	}

cleanup:
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
