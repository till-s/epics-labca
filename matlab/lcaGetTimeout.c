/* $Id: lcaGetTimeout.c,v 1.2 2003/12/23 23:06:56 strauman Exp $ */

/* matlab wrapper for ezcaGetTimeout */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	buildPVs(0,0); /* enforce initialization of mglue library */

	if ( 1 < nlhs ) {
		MEXERRPRINTF("Need one output arg");
		return;
	}

	if ( 0 != nrhs ) {
		MEXERRPRINTF("Expected no rhs argument");
		return;
	}


	if ( ! (plhs[0] = mxCreateDoubleMatrix( 1, 1, mxREAL )) ) {
		MEXERRPRINTF("Not enough memory");
		return;
	}

	*mxGetPr(plhs[0]) = ezcaGetTimeout();
}
