/* $Id: ini.cc,v 1.8 2004/01/28 05:47:40 till Exp $ */

/* xlabcaglue library initializer */

#include <cadef.h>
#include <ezca.h>
#include <multiEzca.h>
#include <multiEzcaCtrlC.h>

#define DEF_TIMEOUT 0.2
#define DEF_RETRIES 20

static int
ezlibinit()
{
CtrlCStateRec saved;

/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
mexPrintf((char*)"Initializing labCA Release '$Name:  $'...\n");
mexPrintf((char*)"Author: Till Straumann <strauman@slac.stanford.edu>\n");

multi_ezca_ctrlC_initialize();

multi_ezca_ctrlC_prologue(&saved);
ezcaAutoErrorMessageOff(); /* calls ezca init() */
multi_ezca_ctrlC_epilogue(&saved);

mexPrintf((char*)"Polling interval is %.2fs; max. timeout: %.2fs\n",
				DEF_TIMEOUT, DEF_RETRIES*DEF_TIMEOUT);
ezcaSetTimeout((float)DEF_TIMEOUT);
ezcaSetRetryCount(DEF_RETRIES);
return 0;
}

static int dummy = ezlibinit();
