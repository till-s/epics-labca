/* $Id: ini.cc,v 1.4 2003/12/23 23:15:56 strauman Exp $ */

/* xlabcaglue library initializer */

#include <cadef.h>
#include <ezca.h>
#include <mex.h>

static int
ezlibinit()
{
/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
mexPrintf((char*)"Initializing labCA Release '$Name:  $'...\n");
mexPrintf((char*)"Author: Till Straumann <strauman@slac.stanford.edu>\n");
ezcaAutoErrorMessageOff();
ezcaSetTimeout((float)0.2);
ezcaSetRetryCount(20);
return 0;
}

static int dummy = ezlibinit();
