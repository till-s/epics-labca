/* $Id: ini.cc,v 1.11 2004/02/12 00:23:58 till Exp $ */

/* xlabcaglue library initializer */

#include <mex.h>
#include <cadef.h>
#include <ezca.h>
#include <multiEzcaCtrlC.h>

class multiEzcaInitializer {
public:
	multiEzcaInitializer();
	~multiEzcaInitializer();
};

multiEzcaInitializer::
multiEzcaInitializer()
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
}

multiEzcaInitializer::
~multiEzcaInitializer()
{
}

static multiEzcaInitializer theini;

