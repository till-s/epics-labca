/* $Id: mglue.c,v 1.12 2004/02/27 01:21:44 till Exp $ */

/* MATLAB - EZCA interface glue utilites */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <cadef.h>
#include <ezca.h>

#define epicsExportSharedSymbols
#include "shareLib.h"
#include "multiEzca.h"
#include "mglue.h"

void epicsShareAPI
releasePVs(PVs *pvs)
{
int i;
	if ( pvs && pvs->names ) {
		for ( i=0; i<pvs->m; i++ ) {
			mxFree(pvs->names[i]);
		}
		mxFree( pvs->names );
		multi_ezca_ctrlC_epilogue( &pvs->ctrlc );	
	}
}

int epicsShareAPI
buildPVs(const mxArray *pin, PVs *pvs)
{
char	**mem = 0;
int     i,m,buflen;
const mxArray *tmp;
int	rval = -1;

	if ( !pvs )
		return -1;

	pvs->names = 0;
	pvs->m     = 0;

	if ( !pin )
		return -1;

	if ( mxIsCell(pin) &&  1 != mxGetN(pin) ) {
			MEXERRPRINTF("Need a column vector of PV names\n");
			goto cleanup;
	}

	m = mxIsCell(pin) ? mxGetM(pin) : 1;

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
	}

	pvs->names = mem;
	pvs->m     = m;
	rval       = 0;

	multi_ezca_ctrlC_prologue(&pvs->ctrlc);

	pvs        = 0;

cleanup:
	releasePVs(pvs);
	return rval;	
}

char epicsShareAPI
marg2ezcaType(const mxArray *typearg)
{
char typestr[2] = { 0 };

	if ( ! mxIsChar(typearg) ) {
		MEXERRPRINTF("(optional) type argument must be a string");
	} else {
		mxGetString( typearg, typestr, sizeof(typestr) );
		switch ( toupper(typestr[0]) ) {
			default:
				MEXERRPRINTF("argument specifies an invalid data type");
 				return ezcaInvalid;

			case 'N':	return ezcaNative;
			case 'B':	return ezcaByte;
			case 'S':	return ezcaShort;
			case 'I':
			case 'L':	return ezcaLong;
			case 'F':	return ezcaFloat;
			case 'D':	return ezcaDouble;
			case 'C':	return ezcaString;
		}
	}
	return ezcaInvalid;
}

void epicsShareAPI
flagError(int nlhs, mxArray *plhs[])
{
int i;
	if ( nlhs ) {
		for ( i=0; i<nlhs; i++ ) {
			mxDestroyArray(plhs[i]);
			/* hope this doesn't fail... */
			plhs[i] = 0;
		}
		mexErrMsgTxt("(see above error messages)");
	}
}
