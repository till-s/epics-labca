/* $Id: lcaSetSeverityWarnLevel.c,v 1.2 2004/01/09 04:10:44 till Exp $ */

/* matlab wrapper for ezcaSetSeverityWarnLevel */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	buildPVs(0,0); /* enforce initialization of mglue library */

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
