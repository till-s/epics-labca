/* $Id: lcaNewMonitorValue.c,v 1.5 2007/05/23 02:50:22 strauman Exp $ */

/* matlab wrapper for ezcaNewMonitorWait */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2007 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int     i   = 0;
PVs     pvs = { {0} };
char	type = ezcaNative;
LcaError theErr;

	lcaErrorInit(&theErr);

	if ( nlhs == 0 )
		nlhs = 1;

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	if ( nlhs > 1 ) {
		lcaRecordError(EZCA_INVALIDARG, "Too many output args", &theErr);
		goto cleanup;
	}

	if ( nrhs < 1 || nrhs > 2 ) {
		lcaRecordError(EZCA_INVALIDARG, "Expected 1..2 rhs argument", &theErr);
		goto cleanup;
	}

	/* check for an optional data type argument */
	if ( nrhs > 1 ) {
		if ( ezcaInvalid == (type = marg2ezcaType( prhs[1], &theErr )) ) {
			goto cleanup;
		}
	}

	if ( buildPVs(prhs[0], &pvs, &theErr) )
		goto cleanup;

    if ( 0 == multi_ezca_wait_mon( pvs.names, pvs.m, type, &theErr ) ) {
		nlhs = 0;
	}

cleanup:
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
