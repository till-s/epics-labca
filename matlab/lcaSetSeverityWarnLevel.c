/* $Id: ezcaSetSeverityWarnLevel.c,v 1.1 2003/12/24 17:30:52 till Exp $ */

/* matlab wrapper for ezcaSetSeverityWarnLevel */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if ( 1 < nlhs ) {
		MEXERRPRINTF("Need one output arg");
		return;
	}

	if ( 1 != nrhs ) {
		MEXERRPRINTF("Expected one rhs argument");
		return;
	}

	if ( !mxIsNumeric(prhs[0]) || 1 != mxGetM(prhs[0]) || 1 != mxGetN(prhs[0]) ) {
		MEXERRPRINTF("Need a single numeric argument");
		return;
	}
	ezcaSetSeverityWarnLevel((int)mxGetScalar(prhs[0]));
}
