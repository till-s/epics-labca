/* $Id: lcaPutNoWait.c,v 1.1 2004/03/23 23:52:07 till Exp $ */

/* matlab wrapper for ezcaPutOldCa */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	theLcaPutMexFunction(nlhs,plhs,nrhs,prhs,0);
}
