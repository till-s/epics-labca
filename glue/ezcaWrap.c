#include <stdio.h>
#ifndef MACHHACK
#include <routines/machine.h> /* fortran/C name conversion for scilab */
#else
#define C2F(name) name##_
#endif

/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>

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
	ezcaDebugOn();
}

void
C2F(ezcadebugoff)(void)
{
	ezcaDebugOff();
}

void
C2F(ezcalibinit)(void)
{
/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
/* allow processing of beacons by background tasks - this
 * is actually quite ugly since libEzca is probably not
 * thread safe!
 */
/*ca_context_create(ca_enable_preemptive_callback);*/
ezcaAutoErrorMessageOff();
fprintf(stderr,"HELLO\n");
ezcaSetTimeout(0.1);
ezcaSetRetryCount(40);
}


#if 0
typedef int buf_t;
#define DBR_BUFT DBR_LONG
#define VALFLD	SVAL

void
C2F(getecdr)(char *recnm, int *l, int **res, int *n)
{
chid nbrd_id, blsz_id, bidx_id,val_id;
long    *ptr,i,nbrd,blsz,zero=0,nord;
buf_t   *buf=0;
char    *nam=0;

*res=0;
*n=0;

nam=malloc(*l + 50);

if ((sprintf(nam,"%s.%s",recnm,VALFLD), ECA_NORMAL!=ca_search(nam,&val_id)) ||
    (sprintf(nam,"%s.NBRD",recnm), ECA_NORMAL!=ca_search(nam,&nbrd_id)) ||
    (sprintf(nam,"%s.BLSZ",recnm), ECA_NORMAL!=ca_search(nam,&blsz_id)) ||
    (sprintf(nam,"%s.BIDX",recnm), ECA_NORMAL!=ca_search(nam,&bidx_id)) ||
    (ECA_NORMAL!=ca_pend_io(5.)) ) {
        printf("PVs for '%s' not found\n",argv[1]);
	goto cleanup;
}

if ((ECA_NORMAL!=ca_array_get(DBR_LONG, 1, nbrd_id, &nbrd)) ||
    (ECA_NORMAL!=ca_array_get(DBR_LONG, 1, blsz_id, &blsz)) ||
/* reset index */
    (ECA_NORMAL!=ca_array_put(DBR_LONG, 1, bidx_id, &zero)) ||
    (ECA_NORMAL!=ca_pend_io(5.))) {
        printf("Unable to get array parameters\n");
        exit(1);
}
buf = (buf_t*)malloc(nbrd + blsz);
cout("start reading data...\n");
for (i=0; i<nbrd; i+=blsz) {
        if (ECA_NORMAL!=ca_array_get(DBR_BUFT, blsz/sizeof(buf_t), val_id, (char*)buf+i)) {
                printf("Error getting block %i\n",i);
                exit(1);
        }
/*      ca_poll(); */
}
if ((ECA_NORMAL!=ca_pend_io(5.))) {
        cerro("Unable to get value (PEND)\n");
	goto cleanup;
}
printf("done.\n");
for(i=0; i< 20; i++)
        printf("%i\n",buf[i]);
for(i=0; i<nbrd/sizeof(buf_t); i++)
        if (i!=buf[i]) {
                printf("mismatch at %i\n",i);
                exit(1);
        }
printf("read of %i elements was successful\n",nbrd/sizeof(buf_t));
	*res = buf;
	buf = 0;
	*n = nbrd/sizeof(buf_t);
cleanup:
	if (nam) free(nam);
	if (buf) free(buf);
}
#endif
