/* $Id: ini.cc,v 1.6 2004/01/14 02:49:47 till Exp $ */

/* xlabcaglue library initializer */

#include <cadef.h>
#include <ezca.h>
#include <multiEzca.h>

#define DEF_TIMEOUT 0.2
#define DEF_RETRIES 20

static int
ezlibinit()
{

/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
mexPrintf((char*)"Initializing labCA Release '$Name:  $'...\n");
mexPrintf((char*)"Author: Till Straumann <strauman@slac.stanford.edu>\n");

multi_ezca_ctrlC_initialize();

ezcaAutoErrorMessageOff(); /* calls ezca init() */

#if defined(WIN32) || defined(_WIN32)
/* Install a callback to detect windoze activity (Ctrl-C) */
ezcaPollCbInstall( multi_ezca_pollCb );
#endif

mexPrintf((char*)"Polling interval is %.2fs; max. timeout: %.2fs\n",
				DEF_TIMEOUT, DEF_RETRIES*DEF_TIMEOUT);
ezcaSetTimeout((float)DEF_TIMEOUT);
ezcaSetRetryCount(DEF_RETRIES);
return 0;
}

static int dummy = ezlibinit();
