/* $Id: ini.cc,v 1.20 2006/04/11 02:18:29 till Exp $ */

/* xlabcaglue library initializer */

#include <mex.h>
#include <cadef.h>
#include <ezca.h>
#include <multiEzcaCtrlC.h>
#include <multiEzca.h>

#if BASE_IS_MIN_VERSION(3,14,7)
#include <stdlib.h>
#include <epicsExit.h>
#endif

/* This is now done via the '-z nodelete' linker option */
#define USE_DLOPEN
#if !defined(WIN32) && !defined(_WIN32) && defined(MATLAB_APP) && defined(USE_DLOPEN)
/* matlab tries to unload mex files and dependent libraries when terminating - something
 * we want to prevent as ezca/CA cannot be shutdown cleanly.
 */
#include <dlfcn.h>
#endif

#ifdef SCILAB_APP
#include <signal.h>
#endif
#if BASE_IS_MIN_VERSION(3,14,7)
void do_epics_exit()
{
	epicsExit(0);
}
#endif

class multiEzcaInitializer {
public:
	multiEzcaInitializer();
#if BASE_IS_MIN_VERSION(3,14,7)
	~multiEzcaInitializer()
		{ do_epics_exit(); }
	;
#endif
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

#if !defined(WIN32) && !defined(_WIN32) && defined(MATLAB_APP) && defined(USE_DLOPEN) && !BASE_IS_MIN_VERSION(3,14,7)
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
#if BASE_IS_MIN_VERSION(3,14,7)
atexit(do_epics_exit);
#endif

}

#if 0 /* this doesn't work properly */
multiEzcaInitializer::
~multiEzcaInitializer()
{
multi_ezca_clear_channels(0,-1);
ezcaLock();
ca_context_destroy();
mexPrintf("Leaving labca finalizer\n");
}
#endif

static multiEzcaInitializer theini;

