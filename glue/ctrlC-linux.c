/* $Id$ */

/* Ctrl-C processing for WIN32 */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2004
 * This file is subject to the EPICS open license, consult the LICENSE file for
 * more information. This header must not be removed.
 */

#include <cadef.h>
#include <ezca.h>
#include <multiEzca.h>
#include <epicsVersion.h>

#if (EPICS_VERSION > 3 || (EPICS_VERSION == 3 && EPICS_REVISION >= 14))
#define EPICS_THREE_FOURTEEN
#endif

#include <signal.h>
#include <stdio.h>

#ifdef EPICS_THREE_FOURTEEN
#include <epicsThread.h>
#include <unistd.h>

static unsigned pidd=0;

static void msk_int(sigset_t *pm)
{
	sigemptyset(pm);
	sigaddset(pm, SIGINT);
}

static sigset_t block_int()
{
sigset_t nm,om;
	msk_int(&nm);
	pthread_sigmask(SIG_BLOCK, &nm, &om);
	return om;
}

static void
sigthread(void *arg)
{
sigset_t om, nm;

	msk_int(&om);
	
	fprintf(stderr,"SIG thread id: 0x%08x, mask 0x%08x\n", pthread_self(), om);
	while (1) {
		if ( sigwait(&om, &nm) )
			perror("sigthread usleep");
		fprintf(stderr,"SIG Thread got a signal\n");
		handler(SIGINT);
	}
}

#else
#define epicsThreadGetIdSelf() 0
#endif

void (*origHandler)(int num) = 0;
static volatile int inside = 0;

static void
handler(int num)
{

fprintf(stderr,"thread 0x%08x got signal\n",pthread_self());

block_int();

if ( SIG_ERR == signal(SIGINT, handler) )
	perror("reinstalling handler");

if (inside)
	ezcaAbort(); 
else if (origHandler)
	origHandler(num);

if ( SIG_ERR == signal(SIGINT, handler) )
	perror("reinstalling handler");
}

unsigned long
multi_ezca_ctrlC_prologue()
{
#if 0
unsigned long rval = (unsigned long) signal(SIGINT, handler);
	return rval;
#else
    return inside = 1;
#endif
}

void
multi_ezca_ctrlC_epilogue(unsigned long restore)
{
#if 0
void (*old)(int) = (void (*)(int)) restore;
	if ( SIG_ERR != old ) {
		signal(SIGINT, old);
	}
#else
	inside = 0;
#endif
}

void
multi_ezca_ctrlC_initialize()
{
#ifdef EPICS_THREE_FOURTEEN
sigset_t om;
	om = block_int();
	epicsThreadCreate("sighdlr",epicsThreadPriorityMax,epicsThreadStackSmall,
			sigthread, 0);
	fprintf(stderr,"ctrlC-init: current thread: 0x%08x, mask 0x%08x\n",pthread_self(),om);
	if ( SIG_ERR == (origHandler = signal(SIGINT, handler)) ) {
		perror("installing signal handler");
		origHandler = 0;
	}
	fprintf(stderr,"Orig handler was 0x%x\n",origHandler);
#endif
}
