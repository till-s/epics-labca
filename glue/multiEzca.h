#ifndef MULTI_EZCA_WRAPPER_H
#define MULTI_EZCA_WRAPPER_H
/* $Id: multiEzca.h,v 1.18 2004/02/11 23:04:56 till Exp $ */

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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef epicsExportSharedSymbols
#	define multi_ezca_epics_ExportSharedSymbols
#	undef epicsExportSharedSymbols
#endif

/* CA includes */
#include <tsDefs.h> 

#ifdef MATLAB_APP
#define cerro(arg) mexPrintf("Error: %s\n",arg)
#define malloc(arg) mxMalloc((arg))
#define free(arg)   mxFree((arg))
#define calloc(n,s) mxCalloc((n),(s))
#else
extern void cerro(const char*);
#endif

#ifdef multi_ezca_epics_ExportSharedSymbols
#  define epicsExportSharedSymbols
#  include <shareLib.h>
#endif

epicsShareFunc void epicsShareAPI
ezcaSetSeverityWarnLevel(int level);

/* MACROS */
#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

/* extra hacking - we store time into a complex number.
 * convert an array of timestamps into two arrays of 
 * doubles holding the real (secs) and imaginary (nanosecs)
 * parts, respectively.
 */

epicsShareFunc void epicsShareAPI
multi_ezca_ts_cvt(int m, TS_STAMP *pts, double *pre, double *pim);

epicsShareFunc int epicsShareAPI
multi_ezca_get_nelem(char **nms, int m, int *dims);

#define ezcaNative  ((char)-1)
#define ezcaInvalid ((char)-2)

epicsShareFunc int epicsShareAPI
multi_ezca_put(char **nms, int m, char type, void *fbuf, int mo, int n);

epicsShareFunc int epicsShareAPI
multi_ezca_get(char **nms, char *type, void **pres, int m, int *pn, TS_STAMP **pts);

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

typedef epicsShareFunc int (epicsShareAPI *MultiEzcaFunc)();

epicsShareFunc int epicsShareAPI
multi_ezca_get_misc(char **nms, int m, MultiEzcaFunc ezcaProc, int nargs, MultiArg args);

#ifdef __cplusplus
};
#endif

#endif
