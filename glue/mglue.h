#ifndef  MATLAB_EZCA_GLUE_H
/* $Id$ */

/* matlab-ezca interface utility header */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */
#include <mex.h>

typedef struct PVs_ {
	int    m;
	char **names;
} PVs;

void
releasePVs(PVs *pvs);

int
buildPVs(const mxArray *pin, PVs *pvs);

/* mexErrMsgTxt() crashes matlab14-beta :-( */
#define MEXERRPRINTF(fmt...) mexWarnMsgTxt(fmt)

#endif

