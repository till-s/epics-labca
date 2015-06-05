/* $Id: lcaSetSeverityWarnLevel.c,v 1.5 2007/05/31 21:16:45 till Exp $ */

/* matlab wrapper for ezcaDebugOn() */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

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

	if ( 0 != nrhs ) {
		lcaSetError(&theErr, EZCA_INVALIDARG, "Expected no rhs argument");
		goto cleanup;
	}

	ezcaDebugOn();

	nlhs = 0;

cleanup:
	ERR_CHECK(nlhs, plhs, &theErr);
}
