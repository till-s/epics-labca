/* $Id: ezcaWrap.c,v 1.14 2003/12/23 22:01:03 till Exp $ */

/* SCILAB - EZCA interface */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003 */

#ifndef MACHHACK
#include <mex.h> /* fortran/C name conversion for scilab */
#else
#define C2F(name) name##_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>


/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>
#include <alarm.h>

#include "multiEzca.h"

void
C2F(ezget)(char ***nms, int *m, int *n, char *type, double **buf, int *mo, int *no, TimeArg *pre, TimeArg *pim, int *hasImag, int *one)
{
char typenum = ezcaDouble;

	*no  = 0;
	*buf = 0;
	*mo  = 0;

	*one = 1;

	/* strange quirk: sometimes intersci doesn't get dimensions right if
	 *                we try to use a single scilab variable for 'm' and 'mo'
	 *                in the 'desc' file...
	 */


	switch ( toupper(type[0]) ) {
		case 'D': typenum = ezcaDouble; break;
		case 'F': typenum = ezcaFloat;  break;
		case 'L': typenum = ezcaLong;   break;
		case 'S': typenum = ezcaShort;  break;
		case 'B': typenum = ezcaByte;   break;
		case 'N': typenum = -1;			break;

		default: 
			cerro("Invalid type - must be 'byte', 'short', 'long', 'float' or 'double'");
		return;
	}

	*no = *n;

	*mo = multi_ezca_get(*nms, typenum, (void**)buf, *m, no, pre, pim, hasImag);
}

void
C2F(ezgetstring)(char ***nms, int *m, int *n, char ***buf, int *mo, int *no, TimeArg *pre, TimeArg *pim, int *hasImag, int *one)
{
	*one = 1;
	*no = *n;
	*mo = multi_ezca_get(*nms, ezcaString, (void**)buf, *m, no, pre, pim, hasImag);
}


void
C2F(ezput)(char ***nms, int *m, char *type, double *val, int *mo, int *n)
{
char typenum;

	switch ( toupper(type[0]) ) {
		case 'B':	typenum = ezcaByte;   break;
		case 'S':	typenum = ezcaShort;  break;
		case 'L':	typenum = ezcaLong;   break;
		case 'F':	typenum = ezcaFloat;  break;
		case 'D':	typenum = ezcaDouble; break;
		case 'N':   typenum = -1;	      break; /* use native type */

		default:
			cerro("Invalid type - must be 'byte', 'short', 'long', 'float' or 'double'");
		return;
	}

	multi_ezca_put(*nms, *m, typenum, val, *mo, *n);
}

void
C2F(ezputstring)(char ***nms, int *m, char ***buf, int *mo, int *n)
{
	multi_ezca_put(*nms, *m, ezcaString, *buf, *mo, *n);
}

void
C2F(ezgetnelem)(char ***nms, int *m, int **pnelms, int *mo)
{
int *nelms = 0;

	*mo     = 0;
	*pnelms = 0;
	if ( ! (nelms = calloc( *m, sizeof(*nelms) )) ) {
		cerro("no memory");	
		goto cleanup;
	}

	if ( multi_ezca_get_nelem( *nms, *m, nelms ) )
		goto cleanup;

	*pnelms = nelms; nelms = 0;
	*mo     = *m;

cleanup:
	free(nelms);
}

void
C2F(ezgetcontrollimits)(char ***nms, int *m, double **plo, double **phi, int *mo)
{
MultiArgRec args[2];

	MSetArg(args[0], sizeof(double), 0, plo);
	MSetArg(args[1], sizeof(double), 0, phi);

    *mo = multi_ezca_get_misc(*nms, *m, ezcaGetControlLimits, NumberOf(args), args);

}

void
C2F(ezgetgraphiclimits)(char ***nms, int *m, double **plo, double **phi, int *mo)
{
MultiArgRec args[2];
	MSetArg(args[0], sizeof(double), 0, plo);
	MSetArg(args[1], sizeof(double), 0, phi);
    *mo = multi_ezca_get_misc(*nms, *m, ezcaGetGraphicLimits, NumberOf(args), args);
}

void
C2F(ezgetstatus)(char ***nms, int *m, TimeArg *pre, TimeArg *pim, int *hasImag, int *one, short **sta, short **svr, int *mo)
{
TS_STAMP *ts;
MultiArgRec args[3];

	MSetArg(args[0], sizeof(TS_STAMP), 0, &ts);
	MSetArg(args[1], sizeof(short),    0, sta);
	MSetArg(args[2], sizeof(short),    0, svr);

    *one = 1;

	if ( timeArgsAlloc(pre,pim,hasImag) )
		return;

	if ( (*mo = multi_ezca_get_misc(*nms, *m, ezcaGetStatus, NumberOf(args), args)) )
		(*pre)->pts = (*pim)->pts = ts;
	else
		timeArgsRelease(pre,pim,hasImag);
}

void
C2F(ezgetprecision)(char ***nms, int *m, short **prec, int *mo)
{
MultiArgRec args[1];

	MSetArg(args[0], sizeof(short), 0, prec);

	*mo = multi_ezca_get_misc(*nms, *m, ezcaGetPrecision, NumberOf(args), args);
}

typedef char units_string[EZCA_UNITS_SIZE];

void
C2F(ezgetunits)(char ***nms, int *m, char ***units, int *mo)
{
units_string	*unitsbuf = 0;
MultiArgRec		args[1];
char			**buf = 0;
int				i,dim;

	MSetArg(args[0], sizeof(units_string), 0, &unitsbuf);

	*units = 0;
	*mo    = 0;

	if ( (dim = multi_ezca_get_misc(*nms, *m, ezcaGetUnits, NumberOf(args), args)) ) {
		if ( !(buf = calloc( *m, sizeof(*buf) )) ) {
			cerro("no memory");
			goto cleanup;
		}

		for ( i=0; i<*m; i++ )
			if ( !(buf[i] = strdup( &unitsbuf[i][0] )) ) {
				cerro("no memory");
				goto cleanup;
			}

		*units = buf; buf = 0;
		*mo    = dim; 
	}

cleanup:
	if ( buf ) {
		for ( i=0; i<*m; i++)
			free(buf[i]);
		free(buf);
	}
	free(unitsbuf);
}

void
C2F(ezgetretrycount)(int *count)
{
	*count = ezcaGetRetryCount();
}

void
C2F(ezsetretrycount)(int *count)
{
	ezcaSetRetryCount(*count);
}

void
C2F(ezgettimeout)(float *tout)
{
	*tout = ezcaGetTimeout();
}

void
C2F(ezsettimeout)(float *tout)
{
	ezcaSetTimeout(*tout);
}

void
C2F(ezdebugon)(void)
{
	ezcaDebugOn();
}

void
C2F(ezdebugoff)(void)
{
	ezcaDebugOff();
}

void
C2F(ezsetseveritywarnlevel)(int *level)
{
	ezcaSeverityWarnLevel = *level;
}
