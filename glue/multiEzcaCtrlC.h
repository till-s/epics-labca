#ifndef MULTI_EZCA_CTRLC_H
#define MULTI_EZCA_CTRLC_H
/* $Id: multiEzca.h,v 1.14 2004/01/28 05:46:42 till Exp $ */

/* interface to Ctrl-C handling */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <signal.h>

typedef struct CtrlCStateRec_ {
	void		(*handler)(int);
	sigset_t	mask;
} CtrlCStateRec, *CtrlCState;

#ifdef __cplusplus
extern "C" {
#endif

void
multi_ezca_ctrlC_prologue(CtrlCState psave);

void
multi_ezca_ctrlC_epilogue(CtrlCState prestore);

/* initialize CtrlC handling; must be
 * called _before_ initializing CA!
 */
void
multi_ezca_ctrlC_initialize();

#ifdef __cplusplus
};
#endif

#endif
