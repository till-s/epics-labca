/* $Id: mglue.c,v 1.2 2003/12/12 10:28:21 till Exp $ */

/* MATLAB - EZCA interface glue utilites */
/* Author: Till Straumann <strauman@slac.stanford.edu> */

#include "mglue.h"
#include "multiEzca.h"
#include <cadef.h>
#include <ezca.h>

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
char	**mem = 0;
int     i,m,buflen;
const mxArray *tmp;
int	rval = -1;
	
	pvs->names = 0;
	pvs->m     = 0;

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
	pvs  = 0;
	rval = 0;

cleanup:
	releasePVs(pvs);
	return rval;	
}

char
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


