/* $Id: ini.cc,v 1.13 2004/03/31 00:32:45 till Exp $ */

/* xlabcaglue library initializer */

#include <mex.h>
#include <cadef.h>
#include <ezca.h>
#include <multiEzcaCtrlC.h>
#if !defined(WIN32) && !defined(_WIN32) && defined(MATLAB_APP)
#include <dlfcn.h>
#endif

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

#if !defined(WIN32) && !defined(_WIN32) && defined(MATLAB_APP)
/* matlab tries to unload mex files and dependent libraries when terminating.
 * CA is not really prepared for a clean shutdown, so we lock our library
 * stack into memory
 */
if ( !dlopen("libmezcaglue.so",RTLD_NOW) ) {
	mexPrintf((char*)"Locking library in memory failed: %s\n",dlerror());
}
#endif

multi_ezca_ctrlC_initialize();

multi_ezca_ctrlC_prologue(&saved);
ezcaAutoErrorMessageOff(); /* calls ezca init() */
multi_ezca_ctrlC_epilogue(&saved);
}

multiEzcaInitializer::
~multiEzcaInitializer()
{
ezcaLock();
ca_context_destroy();
mexPrintf("Leaving labca finalizer\n");
}

static multiEzcaInitializer theini;

