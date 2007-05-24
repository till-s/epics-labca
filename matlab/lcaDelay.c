/* $Id: lcaDelay.c,v 1.4 2007/05/23 02:50:21 strauman Exp $ */

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

	buildPVs(0,0,&theErr); /* enforce initialization of mglue library */

	if ( 1 < nlhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Need one output arg", &theErr);
		return;
	}
	nlhs = 1;

	if ( 1 != nrhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Expected one rhs argument", &theErr);
		return;
	}

	if ( !mxIsDouble(prhs[0]) || 1 != mxGetM(prhs[0]) || 1 != mxGetN(prhs[0]) ) {
		lcaRecordError(EZCA_INVALIDARG, "Need a single numeric argument", &theErr);
		return;
	}

	if ( !ezcaDelay((float)*mxGetPr(prhs[0])) ) {
		nlhs = 0;
	} else {
		lcaRecordError(EZCA_INVALIDARG, "Need a timeout argument > 0.0\n", &theErr);
	}

	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
