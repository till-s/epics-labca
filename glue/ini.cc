/* $Id: ini.cc,v 1.21 2006/04/12 02:15:06 strauman Exp $ */

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

#if defined(WIN32) || defined(_WIN32)
typedef int pid_t;
#define getpid _getpid
extern "C" int _getpid();
#else
#include <unistd.h>
#endif

#ifdef SCILAB_APP
#include <signal.h>
#endif

#undef  DEBUG

#define USE_DLOPEN

#if defined(WIN32) || defined(_WIN32) || BASE_IS_MIN_VERSION(3,14,7)
#undef USE_DLOPEN
#endif

#if defined(USE_DLOPEN)
/* matlab tries to unload mex files and dependent libraries when terminating - something
 * we want to prevent as ezca/CA cannot be shutdown cleanly.
 */
#include <dlfcn.h>
#endif

static	pid_t thepid;

static void
multiEzcaFinalizer()
{
	if ( thepid != getpid() ) {
		/* DONT run this if a forked caRepeater exits */
#ifdef DEBUG
		mexPrintf("labca finalizer: PID mismatch\n");
#endif
		return;
	}
	multi_ezca_clear_channels(0,-1);
	ezcaLock();
#if (!defined(WIN32) && !defined(_WIN32)) || defined(MATLAB_APP)
	/* ca_context_destroy() hangs (scilab/win32) :-( */
	ca_context_destroy();
#endif
#if BASE_IS_MIN_VERSION(3,14,7)
	epicsExit(0);
#endif
#ifdef DEBUG
	mexPrintf("Leaving labca finalizer\n");
#endif
}


class multiEzcaInitializer {
public:
	multiEzcaInitializer();
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

#if defined(USE_DLOPEN)
#ifdef MATLAB_APP
/* matlab tries to unload mex files and dependent libraries when terminating.
 * CA is not really prepared for a clean shutdown, so we lock our library
 * stack into memory
 */
  if ( !dlopen("libmezcaglue.so",RTLD_NOW) ) {
	mexPrintf((char*)"Locking library in memory failed: %s\n",dlerror());
  }
#endif
#ifdef SCILAB_APP
  if ( !dlopen("libsezcaglue.so",RTLD_NOW) ) {
	mexPrintf((char*)"Locking library in memory failed: %s\n",dlerror());
  }
#endif
#endif

multi_ezca_ctrlC_initialize();

multi_ezca_ctrlC_prologue(&saved);
ezcaAutoErrorMessageOff(); /* calls ezca init() */
multi_ezca_ctrlC_epilogue(&saved);

/* On unix the class destructor is called too late; atexit seems to
 * give a cleaner shutdown.
 * Don't know how to do this on win32 -- there, the destructor seems
 * to be good enough...
 * Another problem on unix is the 'fork'ed caRepeater calling the
 * finalizer...
 */
thepid = getpid();
atexit(multiEzcaFinalizer);
}

static multiEzcaInitializer theini;
