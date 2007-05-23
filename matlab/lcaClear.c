/* $Id: lcaClear.c,v 1.4 2007-05-21 22:01:50 till Exp $ */

/* matlab wrapper for ezcaClearChannel / ezcaPurge */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
PVs     pvs = { {0} };
char	**s;
int		m = -1;
LcaError theErr;

	lcaErrorInit(&theErr);

	if ( 0 == nlhs )
		nlhs = 1;

	if ( 1 < nlhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Too many lhs args", &theErr);
		goto cleanup;
	}

	if ( 1 < nrhs ) {
		lcaRecordError(EZCA_INVALIDARG, "Too many rhs args", &theErr);
		goto cleanup;
	}


	if ( nrhs > 0 && buildPVs(prhs[0],&pvs, &theErr) )
		goto cleanup;

	if ( (s = pvs.names) )
		m = pvs.m;

#if 0 /* lcaClear('') or lcaClear({''}) do not pass buildPVs() ;=( */
	if ( 1 == m  && 0 == *s[0] ) {
		/* special case: lcaClear('') clears only disconnected channels */
		s = 0;
		m = 0;
	}
#endif

    if ( multi_ezca_clear_channels( s, m, &theErr ) )
		goto cleanup;

	nlhs = 0;

cleanup:
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
