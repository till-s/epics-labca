/* $Id: lcaGetNelem.c,v 1.5 2006/04/12 02:14:19 strauman Exp $ */

/* matlab wrapper for ezcaGetNelem */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
PVs      pvs = { {0} };
LcaError theErr;

	lcaErrorInit(&theErr);

	if ( 0 == nlhs )
		nlhs = 1;

	if ( 1 < nlhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Need one output arg", &theErr);
		goto cleanup;
	}

	if ( 1 != nrhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Expected one rhs argument", &theErr);
		goto cleanup;
	}


	if ( buildPVs(prhs[0],&pvs, &theErr) )
		goto cleanup;

	if ( ! (plhs[0] = mxCreateNumericMatrix( pvs.m, 1, mxINT32_CLASS, mxREAL )) ) {
		lcaRecordError(EZCA_FAILEDMALLOC, "Not enough memory", &theErr);
		goto cleanup;
	}

    if ( multi_ezca_get_nelem( pvs.names, pvs.m, mxGetData(plhs[0]), &theErr) ) {
		goto cleanup;
	}
	nlhs = 0;

cleanup:
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
