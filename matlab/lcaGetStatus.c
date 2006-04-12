/* $Id: lcaGetStatus.c,v 1.3 2004/06/23 01:15:55 till Exp $ */

/* matlab wrapper for ezcaGetStatus */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <cadef.h>
#include <ezca.h>

#include <ctype.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int			i;
PVs			pvs = { {0} };
TS_STAMP	*ts = 0;
MultiArgRec	args[3];
mxArray		*res[2] = {0};
short		*stat = 0, *sevr = 0;

	if ( nlhs == 0 )
		nlhs = 1;

	for ( i=0; i<nlhs; i++ )
		plhs[i] = 0;

	if ( nlhs > NumberOf(args) ) {
		MEXERRPRINTF("Too many output args");
		goto cleanup;
	}

	if ( nrhs != 1 ) {
		MEXERRPRINTF("Expected 1 rhs argument");
		goto cleanup;
	}

	if ( buildPVs(prhs[0],&pvs) )
		goto cleanup;

	/* alloc matrices for status/severity; multi_ezca_get_misc can directly
	 * store results there...
	 */
	if ( !(res[0]=mxCreateNumericMatrix(pvs.m, 1, mxINT16_CLASS, mxREAL)) ||
		 !(res[1]=mxCreateNumericMatrix(pvs.m, 1, mxINT16_CLASS, mxREAL)) ) {
		MEXERRPRINTF("Not enough memory");
		goto cleanup;
	}

	MSetArg( args[0], sizeof(*ts), 0, &ts );					/* timestamp */
	MSetArg( args[1], sizeof(short),  mxGetData(res[0]), 0);	/* status    */
	MSetArg( args[2], sizeof(short),  mxGetData(res[1]), 0);	/* severity  */

    if ( !multi_ezca_get_misc( pvs.names, pvs.m, (MultiEzcaFunc)ezcaGetStatus, NumberOf(args), args) ) {
		goto cleanup;
	}

	/* set severity in result array */
	plhs[0] = res[1]; res[1] = 0;

	if ( nlhs > 1 ) {
		/* add status to result array */
		plhs[1] = res[0]; res[0] = 0;
	}

	/* If requested, generate the timestamp matrix */
	if ( nlhs > 2 ) {
		/* give them the time stamps */
		if ( !(plhs[2] = mxCreateDoubleMatrix(pvs.m,1,mxCOMPLEX)) ) {
			MEXERRPRINTF("Not enough memory");
			goto cleanup;
		}
		multi_ezca_ts_cvt( pvs.m, ts, mxGetPr(plhs[2]), mxGetPi(plhs[2]) );
	}
	nlhs = 0;

cleanup:
	for ( i=0; i<NumberOf(res); i++ ) {
		if ( res[i] )
			mxDestroyArray( res[i] );
	}
	if (ts)
		mxFree(ts);
	if (stat)
		mxFree(stat);
	if (sevr)
		mxFree(sevr);
	releasePVs(&pvs);
	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs);
}
