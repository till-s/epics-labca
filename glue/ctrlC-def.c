/* $Id: ctrlC-def.c,v 1.2 2004/01/29 05:42:28 till Exp $ */

/* default Ctrl-C handling (none) */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2004
 * This file is subject to the EPICS open license, consult the LICENSE file for
 * more information. This header must not be removed.
 */

#include <multiEzca.h>
#include <multiEzcaCtrlC.h>

void
multi_ezca_ctrlC_prologue(CtrlCState psave)
{
}

void
multi_ezca_ctrlC_epilogue(CtrlCState prest)
{
}

void
multi_ezca_ctrlC_initialize()
{
#ifdef CTRLC_DISABLED
	mexPrintf("Ctrl-C handling is disabled (compile-time option)\n");
#else
	mexPrintf("Ctrl-C handling not implemented for this platform, sorry\n");
#endif
}
