#include <stdio.h>
#include <ctype.h>
#ifndef MACHHACK
#include <routines/machine.h> /* fortran/C name conversion for scilab */
#else
#define C2F(name) name##_
#endif

/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>
#include <alarmString.h>

#define CONVERTFRMDBL(totype) \
			{ \
				totype *dst; \
				double *src; \
				for ( src = val, dst = (totype*)buf, i=*n; i > 0; i--) \
					*dst++ = *src++; \
			}


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
C2F(ezput)(char *nm, int *l, char *type, int *ts, double *val, int *n)
{
int typenum, sz, i;
void *buf, *mem = 0;

	switch ( toupper(type[0]) ) {
		case 'S':	typenum = ezcaShort;  sz = sizeof(short);  break;
		case 'L':	typenum = ezcaLong;   sz = sizeof(long);   break;
		case 'F':	typenum = ezcaFloat;  sz = sizeof(float);  break;
		case 'D':	typenum = ezcaDouble; sz = sizeof(double); break;

		default:
			cerro("Invalid type - must be 'short', 'long', 'float' or 'double'");
		return;
	}

	if ( ezcaDouble != typenum ) {
		sz *= *n;

		if ( !(mem = buf = (void*)malloc(sz) ) ) {
			cerro("no memory");
			return;
		}
	}

	switch ( typenum ) {
		case ezcaShort: CONVERTFRMDBL(short); break;
		case ezcaLong:  CONVERTFRMDBL(long);  break;
		case ezcaFloat: CONVERTFRMDBL(float); break;
		default:
			buf = val;
	}
	
	if (ezcaPut(nm,typenum,*n,buf))
		ezErr("ezcaPut");
	free( mem );
}

void
C2F(ezputstring)(char *nm, int *l, char ***buf, int *m, int *n)
{
char *cabuf, *chpt;
int  nstring = *m * *n;
int  i;
	if ( *m != 1 && *n !=1 ) {
		cerro("argument must be a vector");
		return;
	}
	if ( !(cabuf=(char*)malloc( nstring * sizeof(dbr_string_t) )) ) {
		cerro("no memory");
		goto bailout;
	}

	for ( i = 0, chpt = cabuf; i<nstring; i++	, chpt += sizeof(dbr_string_t) ) {
		if ( strlen((*buf)[i])+1 >= sizeof(dbr_string_t) ) {
			cerro("string argument too long");
			goto bailout;
		}
		strcpy( chpt, (*buf)[i] );
	}

	if (ezcaPut(nm, ezcaString, nstring, cabuf))
		ezErr("ezcaPut");

bailout:
	free(cabuf);
	
}

static void do_ezca_get(char *nm, int typenum, void **buf, int *n)
{
TS_STAMP ts;
short    sta = 0,sevr;
int      e=0;
char	 msgbuf[200];

*buf=0;

if ( (e=ezcaGetNelem(nm,n)) ) {
	ezErr("ezcaGetNelem");
	goto bailout;
}

if ( !(*buf = (void*)malloc( (ezcaString == typenum ? sizeof(dbr_string_t) : sizeof(double) ) * *n)) ) {
	cerro("no memory");
	goto bailout;
}

if ( e=ezcaGetWithStatus(nm,typenum,*n,*buf,&ts,&sta,&sevr) ) {
	ezErr("ezcaGet");
	goto bailout;
}

if ( sta ) {
	sprintf(msgbuf,"PV status: %s (severity: %s)",alarmStatusString[sta],alarmSeverityString[sevr]);
	cerro(msgbuf);
	goto bailout;
}

return;

bailout:
	if (*buf) free(*buf);
	*buf = 0;
	*n = 0;
}

#define CONVERT2DBL(fromtype) \
			{ \
				fromtype *src; \
				for ( dst = *buf+*n-1, src = (fromtype*)*buf + *n -1, i=*n; i > 0; i--) \
					*dst-- = (double)*src--; \
			}

void
C2F(ezget)(char *nm, int *l, char *type, int *ls, double **buf, int *n)
{
*buf = 0;
*n   = 0;
int typenum = ezcaDouble;

register double *dst;
register int    i;

switch ( toupper(type[0]) ) {
	case 'D': typenum = ezcaDouble; break;
	case 'F': typenum = ezcaFloat;  break;
	case 'S': typenum = ezcaShort;  break;
	case 'L': typenum = ezcaLong;   break;

	default: 
		cerro("Invalid type - must be 'short', 'long', 'float' or 'double'");
	return;
}
	do_ezca_get(nm, typenum, (void**)buf, n);

if ( *buf ) {
switch (typenum) {
	case ezcaShort: CONVERT2DBL(short); break;
	case ezcaLong:  CONVERT2DBL(long);  break;
	case ezcaFloat: CONVERT2DBL(float); break;
	default:
		break;
}
}

}

void
C2F(ezgetstring)(char *nm, int *l, char ***buf, int *m, int *n)
{
char	*cabuf, *chpt;
int		i;

	*buf = 0;

	do_ezca_get(nm,ezcaString,(void**)&cabuf,m);
if (cabuf)
	printf("Got %i elements, '%s'\n",*m, cabuf);

	if ( cabuf ) {
		/*  unfortunately, we must rearrange the string array */
		if ( ! (*buf = (char **)malloc( sizeof(char*) * *m ) ) ) {
			cerro("no memory");
			goto bailout;
		}
		for ( i=0; i<*m; i++)
			(*buf)[i] = 0;
		for ( i=0, chpt = cabuf; i<*m; i++, chpt += sizeof(dbr_string_t)) {
			if ( ! ((*buf)[i] =  (char*)malloc(strlen(chpt) + 1) ) )
				goto bailout;
			strcpy( (*buf)[i], chpt );
		}
		free(cabuf);
		*n = 1;
		return;
	}

bailout:
	if ( cabuf )
		free(cabuf);
	if ( *buf ) {
		for ( i=0; i<*m; i++)
			free( (*buf)[i] );
		free( *buf );
		*buf = 0;
	}
	*m   = 0;
	*n   = 0;
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
		*sta = sstat; *svr = ssevr;
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
C2F(ezcastartgroup)(void)
{
	if (ezcaStartGroup())
		ezErr("ezcaStartGroup");
}

void
C2F(ezcaendgroup)(void)
{
	if (ezcaEndGroup())
		ezErr("ezcaEndGroup");
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
