#include <stdio.h>
#include <routines/machine.h> /* fortran/C name conversion for scilab */

/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 

/* note that the routines defined here have "C" linkage */
extern "C" {

/* ezca header seems not to be C++ safe */
#include <ezca.h>

static int
ezcaLibIni(void)
{
/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
ezcaAutoErrorMessageOff();
fprintf(stderr,"HELLO\n");
return 1;
}

static int inited=ezcaLibIni();


extern void cerro(char*); /* scilab error */

static void
ezErr(char *nm)
{
	char *msg;
	ezcaGetErrorString(nm,&msg);
	if (msg) cerro(msg);
	ezcaFree(msg);
}

void
C2F(ezput)(char *nm, int *l, double *buf, int *n)
{
ezcaPut(nm,5,*n,buf);
}

void
C2F(ezget)(char *nm, int *l, double **buf, int *n)
{
int e=0;
*buf=0;
if ( (e=ezcaGetNelem(nm,n))
	|| !(*buf=(double*)malloc(sizeof(**buf)* *n))
	|| (e=ezcaGet(nm,5,*n,*buf)) )
	{
	extern void cerro(char*);
	if (*buf) free(*buf);
	*buf=0;
	*n=0;
	if (e) ezErr("ezcaGet");
	else   cerro("no memory");
	};
}

void
C2F(ezgetnelem)(char *nm, int *l, int *n)
{
	if (ezcaGetNelem(nm,n))
		ezErr("ezcaGetNelem");
}

void
C2F(ezgetcontrollimits)(char *nm, int *l, double *lo, double *hi)
{
	if (ezcaGetControlLimits(nm,lo,hi))
		ezErr("ezcaGetControlLimits");
}

void
C2F(ezgetgraphiclimits)(char *nm, int *l, double *lo, double *hi)
{
	if (ezcaGetGraphicLimits(nm,lo,hi))
		ezErr("ezcaGetGraphicLimits");
}

void
C2F(ezgetstatus)(char *nm, int *l, long *ts, long *sta, long *svr)
{
TS_STAMP tmp;
short	sstat,ssevr;
	if (ezcaGetStatus(nm,&tmp,&sstat,&ssevr))
		ezErr("ezcaGetStatus");
	else {
		*sta = sstat;
		*svr = ssevr;
		*ts=tsLocalTime(&tmp);
	}
}

void
C2F(ezcadebugon)(void)
{
	/* note that debugging message appears
         * on the terminal from where scilab
         * was launched rather than on the scilab
         * main window...
         */
	ezcaDebugOn();
}

void
C2F(ezcadebugoff)(void)
{
	ezcaDebugOff();
}

} /* "C" linkage */
