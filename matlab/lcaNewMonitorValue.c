/* $Id: lcaNewMonitorValue.c,v 1.1 2004/06/20 04:25:55 till Exp $ */

/* matlab wrapper for ezcaGet */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int     i   = 0;
PVs     pvs = { 0 };
char	type = ezcaNative;

	if ( nlhs == 0 )
		nlhs = 1;

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	if ( nlhs > 1 ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}

	if ( nrhs < 1 || nrhs > 2 ) {
		MEXERRPRINTF("Expected 1..2 rhs argument");
		goto cleanup;
	}

	/* check for an optional data type argument */
	if ( nrhs > 1 ) {
		if ( ezcaInvalid == (type = marg2ezcaType( prhs[1] )) ) {
			goto cleanup;
		}
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

	if ( ! (plhs[0] = mxCreateNumericMatrix( pvs.m, 1, mxINT32_CLASS, mxREAL )) ) {
		MEXERRPRINTF("Not enough memory");
		goto cleanup;
	}

    multi_ezca_check_mon( pvs.names, pvs.m, type, mxGetData(plhs[0]) );

	nlhs = 0;

cleanup:
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs);
}
