#include <cadef.h>
#include <ezca.h>
#include <mex.h>

static int
ezlibinit()
{
/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
mexPrintf("Hello World\n");
ezcaAutoErrorMessageOff();
ezcaSetTimeout(0.2);
ezcaSetRetryCount(20);
return 0;
}

static int dummy = ezlibinit();
