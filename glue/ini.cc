/* $Id: ini.cc,v 1.9 2004/01/29 05:44:26 till Exp $ */

/* xlabcaglue library initializer */

#include <mex.h>
#include <cadef.h>
#include <ezca.h>
#include <multiEzcaCtrlC.h>

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
return 0;
}

static int dummy = ezlibinit();
