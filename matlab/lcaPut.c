/* $Id: lcaPut.c,v 1.7 2004/06/23 01:15:55 till Exp $ */

/* matlab wrapper for ezcaPut */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;

	lcaErrorInit(&theErr);

	nlhs = theLcaPutMexFunction(nlhs,plhs,nrhs,prhs,1,&theErr);
	ERR_CHECK(nlhs, plhs, &theErr);
}
