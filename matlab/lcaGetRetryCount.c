/* $Id$ */

/* matlab wrapper for ezcaGetRetryCount */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if ( 1 < nlhs ) {
		MEXERRPRINTF("Need one output arg");
		return;
	}

	if ( 0 != nrhs ) {
		MEXERRPRINTF("Expected no rhs argument");
		return;
	}


	if ( ! (plhs[0] = mxCreateNumericMatrix( 1, 1, mxINT32_CLASS, mxREAL )) ) {
		MEXERRPRINTF("Not enough memory");
		return;
	}

	*(int*)mxGetData(plhs[0]) = ezcaGetRetryCount();
}
