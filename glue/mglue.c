/* $Id$ */

/* MATLAB - EZCA interface glue utilites */
/* Author: Till Straumann <strauman@slac.stanford.edu> */

#include "mglue.h"

void
releasePVs(PVs *pvs)
{
int i;
	if ( pvs && pvs->names ) {
		for ( i=0; i<pvs->m; i++ ) {
			mxFree(pvs->names[i]);
		}
		mxFree( pvs->names );
	}
}

int
buildPVs(const mxArray *pin, PVs *pvs)
{
char	**mem = 0, *str;
int     i,m,buflen;
const mxArray *tmp;
int	rval = -1;
	
	pvs->names = 0;
	pvs->m     = 0;

	m = mxIsCell(pin) ? mxGetNumberOfElements(pin) : 1;

	if ( (! mxIsCell(pin) && ! mxIsChar(pin)) || m < 1 ) {
		MEXERRPRINTF("Need a cell array argument with PV names");
		/* GENERAL CLEANUP NOTE: as far as I understand, this is not necessary:
		 *                       in mex files, garbage is automatically collected;
		 *                       explicit cleanup works in standalong apps also, however.
		 */
		goto cleanup;
	}

	if ( ! (mem = mxCalloc(m, sizeof(*mem))) ) {
		MEXERRPRINTF("No Memory\n");
		goto cleanup;
	}


	for ( i=buflen=0; i<m; i++ ) {
		tmp = mxIsCell(pin) ? mxGetCell(pin, i) : pin;		
		if ( 1 != mxGetM(tmp) || ! mxIsChar(tmp) ) {
			MEXERRPRINTF("Not an vector of strings??");
			goto cleanup;
		}
		buflen = mxGetN(tmp) * sizeof(mxChar) + 1;
		if ( !(mem[i] = mxMalloc(buflen)) ) {
			MEXERRPRINTF("No Memory\n");
			goto cleanup;
		}
		if ( mxGetString(tmp, mem[i], buflen) ) {
			MEXERRPRINTF("not a PV name?");
			goto cleanup;
		}
mexPrintf("TSILL %s\n",mem[i]);
	}

	pvs->names = mem;
	pvs->m     = m;
	pvs  = 0;
	rval = 0;

cleanup:
	releasePVs(pvs);
	return rval;	
}
