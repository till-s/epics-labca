#include "mglue.h"

#include <cadef.h>
#include <ezca.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if ( 1 < nlhs ) {
		MEXERRPRINTF("Need one output arg");
		return;
	}

	if ( 0 != nrhs ) {
		MEXERRPRINTF("Expected no rhs argument");
		return;
	}


	if ( ! (plhs[0] = mxCreateDoubleMatrix( 1, 1, mxREAL )) ) {
		MEXERRPRINTF("Not enough memory");
		return;
	}

	*mxGetPr(plhs[0]) = ezcaGetTimeout();
}
