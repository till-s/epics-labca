/* $Id: ini.cc,v 1.24 2007/04/11 00:32:24 till Exp $ */

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

#define  DEBUG

#define USE_DLOPEN
#define USE_ATEXIT

#if defined(WIN32) || defined(_WIN32) || BASE_IS_MIN_VERSION(3,14,7)
#undef USE_DLOPEN
#endif

#if defined(USE_DLOPEN)
/* 
 * we want to prevent unloading dynamic libraries since ezca/CA cannot be shutdown cleanly.
 */
#include <dlfcn.h>
#endif

static	pid_t thepid;

static void
multiEzcaFinalizer()
{
#ifdef DEBUG
	mexPrintf("Entering labca finalizer\n");
#endif
	if ( thepid != getpid() ) {
		/* DONT run this if a forked caRepeater exits */
#ifdef DEBUG
		mexPrintf("labca finalizer: PID mismatch\n");
#endif
		return;
	}
	multi_ezca_clear_channels(0,-1, 0);
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
	~multiEzcaInitializer();
};

multiEzcaInitializer::
~multiEzcaInitializer()
{
#ifndef USE_ATEXIT
	multiEzcaFinalizer();
#endif
}

multiEzcaInitializer::
multiEzcaInitializer()
{
CtrlCStateRec saved;

#ifdef MATLAB_APP
	{
	/* Check if this is executed from an mcc run
	 *
	 * (Thanks to Jim Sebek who suggested this fix
	 * on 4/10/07).
	 */
	mxArray *lhs = NULL;

	if ( 0 == mexCallMATLAB(1, &lhs, 0, NULL, "ismcc") &&
		 *(int*)mxGetData(lhs) ) {
		mexPrintf("no initialization of multiEzcaInitializer in ini.cc during mcc compilation\n");
		return;
	}
	}
#endif

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

#ifdef MATLAB_APP
  /* Under matlab, always lock the library in memory - otherwise, CLEAR ALL & friends
   * would unload us calling finalizer who calls epicsExit() who calls exit :-(
   */
  mexLock();
#endif
#if defined(USE_DLOPEN)
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
#ifdef USE_ATEXIT
atexit(multiEzcaFinalizer);
#endif
}

static multiEzcaInitializer theini;
