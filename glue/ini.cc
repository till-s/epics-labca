/* $Id: ini.cc,v 1.12 2004/03/01 19:36:45 till Exp $ */

/* xlabcaglue library initializer */

#include <mex.h>
#include <cadef.h>
#include <ezca.h>
#include <multiEzcaCtrlC.h>

#ifdef SCILAB_APP
#include <signal.h>
#endif

class multiEzcaInitializer {
public:
	multiEzcaInitializer();
	~multiEzcaInitializer();
};

multiEzcaInitializer::
multiEzcaInitializer()
{
CtrlCStateRec saved;

#ifdef SCILAB_APP
	/* uninstall scilabs recovery method; I'd rather have
	 * a coredump to debug...
	 */
	signal(SIGSEGV, SIG_DFL);
#endif

/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
mexPrintf((char*)"Initializing labCA Release '$Name:  $'...\n");
mexPrintf((char*)"Author: Till Straumann <strauman@slac.stanford.edu>\n");

multi_ezca_ctrlC_initialize();

multi_ezca_ctrlC_prologue(&saved);
ezcaAutoErrorMessageOff(); /* calls ezca init() */
multi_ezca_ctrlC_epilogue(&saved);
}

multiEzcaInitializer::
~multiEzcaInitializer()
{
}

static multiEzcaInitializer theini;

