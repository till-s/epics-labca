/* $Id$ */

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
PVs     pvs = { 0 };

	if ( 1 < nlhs ) {
		MEXERRPRINTF("Need one output arg");
		goto cleanup;
	}

	if ( 1 != nrhs ) {
		MEXERRPRINTF("Expected one rhs argument");
		goto cleanup;
	}


	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

	if ( ! (plhs[0] = mxCreateNumericMatrix( pvs.m, 1, mxINT32_CLASS, mxREAL )) ) {
		MEXERRPRINTF("Not enough memory");
		goto cleanup;
	}

    if ( multi_ezca_get_nelem( pvs.names, pvs.m, mxGetData(plhs[0])) ) {
		mxDestroyArray( plhs[0] );
		plhs[0] = 0;
	}

cleanup:
	releasePVs(&pvs);
}
