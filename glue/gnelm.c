#include <cadef.h>
#include <mglue.h>
#include <multiEzca.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_PVS 25

int
main(int argc, char **argv)
{
int blah[MAX_PVS];
int i;
argv++; 
if (--argc > MAX_PVS)
  argc = MAX_PVS;
if (!argc) {
	fprintf(stderr,"Usage: %s PV [PV]\n",argv[0]);
	exit(1);
}
multi_ezca_get_nelem(argv,argc,blah);
for (i=0; i<argc-1; i++)
	printf("%i\n",blah[i]);

return 0;
}

#if 0
int mexPrintf(char  *fmt, ...)
{
int    rval;
va_list ap;
	va_start(fmt,ap);
	rval = vprintf(fmt,ap);
	va_end(ap);
	return rval;
}
#endif
