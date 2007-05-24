#ifndef  MATLAB_EZCA_GLUE_H
/* $Id: mglue.h,v 1.14 2007/05/23 02:50:15 strauman Exp $ */

/* matlab-ezca interface utility header */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */
#include <mex.h>
#include <matrix.h>
#include <multiEzcaCtrlC.h>
#include <lcaError.h>

typedef struct PVs_ {
	CtrlCStateRec	ctrlc;
	int    			m;
	char 			**names;
} PVs;

#include "shareLib.h"

#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc void epicsShareAPI
releasePVs(PVs *pvs);

epicsShareFunc void epicsShareAPI
lcaRecordError(int rc, char *msg, LcaError *pe);

epicsShareFunc int epicsShareAPI
buildPVs(const mxArray *pin, PVs *pvs, LcaError *pe);

epicsShareFunc const char * epicsShareAPI
lcaErrorIdGet(int err);

/* We don't want to jump out of context; instead,
 * we check for accumulated errors prior to exiting
 * the mexfiles.
 */
#define MEXERRPRINTF(arg) mexPrintf("Error: %s\n",(arg))

/* check for 'typearg' being a string and use the
 * first character to determine the desired ezca type
 *  ezca'N'ative
 *  ezca'B'yte
 *  ezca'S'hort
 *  ezca'L'ong
 *  ezca'F'loat
 *  ezca'D'ouble
 * or 'C'har for ezcaString
 */
epicsShareFunc char epicsShareAPI
marg2ezcaType(const mxArray *typearg, LcaError *pe);

/* use 'nlhs' as an 'error' flag; (jumped out of something and
 * have already assigned 'lhs' args).
 * Clean up the lhs args and flag an error condition.
 * If everyhing's OK, the caller passes nlhs == 0.
 */
epicsShareFunc int epicsShareAPI
flagError(int nlhs, mxArray *plhs[]);

/* mexErrMsgTxt() should be called only from a routine
 * that was compiled with the mex compiler in order to
 * reduce C++ problems (mexErrMsgTxt throws a C++ exception)
 */
#define ERR_CHECK(nlhs, plhs, perr) \
	do { if (flagError((nlhs),(plhs))) { \
			lcaSaveLastError(perr); \
			mexErrMsgIdAndTxt(lcaErrorIdGet((perr)->err), (perr)->msg); \
		 } else { \
		 	free((perr)->errs); (perr)->errs = 0; \
			(perr)->nerrs = 0; \
		 } \
	} while (0)
epicsShareFunc int epicsShareAPI
theLcaPutMexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], int doWait, LcaError *pe);

#ifdef __cplusplus
};
#endif

#endif
