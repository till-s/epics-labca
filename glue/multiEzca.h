#ifndef MULTI_EZCA_WRAPPER_H
#define MULTI_EZCA_WRAPPER_H
/* $Id: multiEzca.h,v 1.12 2004/01/14 00:06:27 till Exp $ */

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

#ifdef MATLAB_APP
#define C2F(name) name
#define cerro(arg) mexPrintf("Error: %s\n",arg)
#define malloc(arg) mxMalloc((arg))
#define free(arg)   mxFree((arg))
#define calloc(n,s) mxCalloc((n),(s))
/* #define C2F(name) name##_ */
#else
void cerro(const char*);
#endif

/* CA includes */
#include <tsDefs.h> 

void ezcaSetSeverityWarnLevel(int level);

/* MACROS */
#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

/* extra hacking - we store time into a complex number.
 * convert an array of timestamps into two arrays of 
 * doubles holding the real (secs) and imaginary (nanosecs)
 * parts, respectively.
 */

void
multi_ezca_ts_cvt(int m, TS_STAMP *pts, double *pre, double *pim);

int
multi_ezca_get_nelem(char **nms, int m, int *dims);

#define ezcaNative  ((char)-1)
#define ezcaInvalid ((char)-2)

void
multi_ezca_put(char **nms, int m, char type, void *fbuf, int mo, int n);

int
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

typedef int (*MultiEzcaFunc)();

int
multi_ezca_get_misc(char **nms, int m, MultiEzcaFunc ezcaProc, int nargs, MultiArg args);

unsigned long
multi_ezca_ctrlC_prologue();

void
multi_ezca_ctrlC_epilogue(unsigned long);

/* initialize CtrlC handling; must be
 * called _before_ initializing CA!
 */
void
multi_ezca_ctrlC_initialize();

#if defined(WIN32) || defined(_WIN32)
int
multi_ezca_pollCb();
#endif

#ifdef __cplusplus
};
#endif

#endif
