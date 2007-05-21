/* $Id: lcaDelay.c,v 1.2 2004/06/23 01:15:54 till Exp $ */

/* matlab wrapper for ezcaDelay */

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
	nlhs = 1;

	if ( 1 != nrhs ) {
		MEXERRPRINTF("Expected one rhs argument");
		return;
	}

	if ( !mxIsDouble(prhs[0]) || 1 != mxGetM(prhs[0]) || 1 != mxGetN(prhs[0]) ) {
		MEXERRPRINTF("Need a single numeric argument");
		return;
	}

	if ( !ezcaDelay((float)*mxGetPr(prhs[0])) ) {
		nlhs = 0;
	} else {
		fprintf(stderr,"Need a timeout argument > 0.0\n");
	}

	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs);
}
