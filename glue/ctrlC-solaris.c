/* $Id$ */

/* Ctrl-C handling for solaris */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2004
 * This file is subject to the EPICS open license, consult the LICENSE file for
 * more information. This header must not be removed.
 */

/* Solaris is nice; signals are just delivered to ONE thread */
#define INST

#include <cadef.h>
#include <ezca.h>
#include <multiEzca.h>

#if 0
#define _POSIX_PTHREAD_SEMANTICS /* solaris */
#endif
#include <signal.h>
#include <stdio.h>

#ifndef INST
static void (*orig)(int)=0;
static int inside = 0;
#endif


typedef void (*SHandler)(int);

static SHandler siginst(SHandler h)
{
struct sigaction sa,so;
	sa.sa_handler = h;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = 0;
	if ( sigaction(SIGINT, &sa, &so) ) {
		perror("installing SIGINT handler");
		return SIG_ERR;
	}
	fprintf(stderr,"siginst: replacing 0x%08x by 0x%08x\n",so.sa_handler, sa.sa_handler);
	return so.sa_handler;
}

static void
handler(int num)
{
#ifdef DEBUG
	fprintf(stderr,"thread 0x%08x got signal %i\n",pthread_self(),num);
#endif

#ifdef INST
	ezcaAbort();
#else
	if (inside)
		ezcaAbort(); 
	else if (orig)
			orig(num);
#endif
}

unsigned long
multi_ezca_ctrlC_prologue()
{
#ifdef DEBUG
	fprintf(stderr,"Entering ctrlC_prologue\n");
#endif
#ifdef INST
	return (unsigned long) siginst(handler);
#else
	inside = 1;
	return 0;
#endif
}

void
multi_ezca_ctrlC_epilogue(unsigned long restore)
{
#ifdef INST
void (*old)(int) = (void (*)(int)) restore;
	if ( SIG_ERR != old ) {
		if ( handler != siginst(old) )
			fprintf(stderr,"restoring old handler failed\n");
	}
#else
	inside = 0;
#endif
#ifdef DEBUG
	fprintf(stderr,"Leaving ctrlC_epilogue\n");
#endif
}

void
multi_ezca_ctrlC_initialize()
{
#ifndef INST
	orig = siginst(handler);
	if ( SIG_ERR == orig )
		orig = 0;
#else
#ifdef SCILAB_APP
	{
	SHandler s = signal(SIGINT, SIG_DFL);
	/* Workaround Scilab BUG: they use 'signal' and don't
	 * let their handler reinstall itself
	 */
	fprintf(stderr, "handler is %x\n",s);
	siginst(s);
	}
#endif
#endif
}
