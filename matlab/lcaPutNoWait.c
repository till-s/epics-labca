/* $Id: lcaPut.c,v 1.5 2004/03/04 21:57:37 till Exp $ */

/* matlab wrapper for ezcaPut */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	theLcaPutMexFunction(nlhs,plhs,nrhs,prhs,0);
}
