/* $Id: lcaPutNoWait.c,v 1.2 2007-05-21 22:01:56 till Exp $ */

/* matlab wrapper for ezcaPutOldCa */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;

	lcaErrorInit(&theErr);
	nlhs = theLcaPutMexFunction(nlhs,plhs,nrhs,prhs,0,&theErr);
	ERR_CHECK(nlhs, plhs, &theErr);
}
