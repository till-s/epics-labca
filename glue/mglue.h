#ifndef  MATLAB_EZCA_GLUE_H
/* $Id: mglue.h,v 1.4 2003/12/23 20:30:52 till Exp $ */

/* matlab-ezca interface utility header */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */
#include <mex.h>
#include <matrix.h>

typedef struct PVs_ {
	int    m;
	char **names;
} PVs;

#ifdef __cplusplus
extern "C" {
#endif

void
releasePVs(PVs *pvs);

int
buildPVs(const mxArray *pin, PVs *pvs);

/* mexErrMsgTxt() crashes matlab14-beta :-( */
#define MEXERRPRINTF mexWarnMsgTxt

void
ecdrget(char *name, int *nlen, long **buf, int *nelms);

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
char
marg2ezcaType(const mxArray *typearg);

#ifdef __cplusplus
};
#endif

#endif

