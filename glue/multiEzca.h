#ifndef MULTI_EZCA_WRAPPER_H
#define MULTI_EZCA_WRAPPER_H
/* $Id: multiEzca.h,v 1.5 2003/12/23 22:08:34 till Exp $ */

/* interface to multi-PV EZCA calls */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include <mex.h> /* fortran/C name conversion for scilab */

#ifdef MATLAB_APP
#define C2F(name) name
#define cerro(arg) mexPrintf("Error: %s\n",arg)
#define malloc(arg) mxMalloc((arg))
#define free(arg)   mxFree((arg))
#define calloc(n,s) mxCalloc((n),(s))
/* #define C2F(name) name##_ */
#else
extern void cerro(const char*);
#endif

/* CA includes */
#include <tsDefs.h> 

/* GLOBAL VARIABLES */
extern int ezcaSeverityWarnLevel;

/* MACROS */
#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

/* scilab external type converters */

void C2F(cshortf)(int *n, short *ip[], double *op);

/* extra hacking - we store time into a complex number.
 * Scilab stores those into two separate arrays and hence this
 * routine is called twice; one for the real part and one more
 * time for the imaginary. Hence, we must keep some state information
 * to do the proper bookkeeping :-(
 */

typedef struct TimeArgRec_ {
	TS_STAMP	*pts;	/* the array of timestamps */
	char		doFree;	/* free timestamp array after use */
	char		imag;   /* called for the real or imaginary part? */
} TimeArgRec, *TimeArg;

int
timeArgsAlloc(TimeArg *pre, TimeArg *pim, int *phasImag);

void
timeArgsRelease(TimeArg *pre, TimeArg *pim, int *phasImag);

void C2F(cts_stampf_)(int *n, TimeArg *ip, double *op);

int
multi_ezca_get_nelem(char **nms, int m, int *dims);

#define ezcaNative  ((char)-1)
#define ezcaInvalid ((char)-2)

void
multi_ezca_put(char **nms, int m, char type, void *fbuf, int mo, int n);

int
multi_ezca_get(char **nms, char type, void **pres, int m, int *pn, TimeArg *pre, TimeArg *pim, int *hasImag);

typedef struct MultiArgRec_ {
	int		size;
	void	*buf;
	void	**pres;
} MultiArgRec, *MultiArg;

#define MSetArg(a, s, b, p) do { \
	(a).size = (s); \
	(a).buf  = (b);  \
    (a).pres = (void**)(p); \
	} while (0)

int
multi_ezca_get_misc(char **nms, int m, int (*ezcaProc)(), int nargs, MultiArg args);

#endif
