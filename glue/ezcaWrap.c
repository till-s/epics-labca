/* $Id$ */

/* SCILAB - EZCA interface */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#ifndef MACHHACK
#include <mex.h> /* fortran/C name conversion for scilab */
#else
#define C2F(name) name##_
#endif

/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>
#include <alarm.h>
#include <alarmString.h>

/* MACROS */
#define NAN (0./0.)
#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

/* GLOBAL VARIABLES */
static int ezcaSeverityWarnLevel = INVALID_ALARM;

#undef TESTING

/* ezca needs a bugfix for correctly reading Nelem in grouped mode */

#define EZCA_BUG_FIXED 1 /* patch applied */

/* in TESTING mode, we fake a couple of ezca routines for debugging
 * conversion and packing issues
 */
#ifdef TESTING
static int ezcaPut(char *name, char type, int nelms, void *bufp)
{
int j;
	printf("%30s:",name);
	for (j=0; j<nelms; j++)
			switch(type) {
				case ezcaShort:  printf(" %i", ((short*)bufp)[j]); break;
				case ezcaLong:   printf(" %i", ((long*)bufp)[j]); break;
				case ezcaFloat:  printf(" %g", ((float*)bufp)[j]); break;
				case ezcaDouble: printf(" %g", ((double*)bufp)[j]); break;
				case ezcaString: printf(" '%s'",&((dbr_string_t*)bufp)[j][0]); break;
			}
	printf("\n");
	return EZCA_OK;
}

typedef struct {
	char *name;
	short num[];
} TstShtRow;

typedef struct {
	char *name;
	long num[];
} TstLngRow;

typedef struct {
	char *name;
	float num[];
} TstFltRow;

static short tstSht[] = {
	10, 20, 30,
	-1, 0xdead, -1,
	-2, -3, -4,
	-5, -6, 0xdead
};

static long tstLng[] = {
	10, 20, 30,
	-1, 0xdead, -1,
	-2, -3, -4,
	-5, -6, 0xdead
};

static float tstFlt[] = {
	.1, .2, .3,
	-1., NAN, -1.,
	-2., -3., -4.,
	-5., -6., NAN
};

static double tstDbl[] = {
	.1, .2, .3,
	-1., NAN, -1.,
	-2., -3., -4.,
	-5., -6., NAN
};

static char *tstChr[] = {
	".1", ".2", ".3",
	"-1.", 0, "-1.",
	"-2.", "-3.", "-4.",
	"-5.", "-6.", 0
};

static int ezcaGetNelem(char *name, int *pn)
{
int t = toupper(*name);
int idx,i;

	while ( !isdigit(*name) && *name )
			name++;

	if (1!=sscanf(name,"%i",&idx) || idx >3) {
		printf("Invalid index\n");
		return -1;
	}

	for ( i=0; i<3; i++)
	switch ( t ) {
			case 'S': if ((short)0xdead == tstSht[3*idx+i]) { *pn = i; return 0; } break;
			case 'L': if (0xdead == tstLng[3*idx+i]) { *pn = i; return 0; } break;
			case 'F': if (isnan(tstFlt[3*idx+i])) { *pn = i; return 0; } break;
			case 'D': if (isnan(tstDbl[3*idx+i])) { *pn = i; return 0; } break;
			case 'C': if (!tstChr[3*idx+i])       { *pn = i; return 0; } break;
	default:
		printf("Invalid fake name \n");
		return -1;
	}
	*pn = 3;
	return 0;
}

static int ezcaGetWithStatus(char *name, char type, int nelms, void *bufp, TS_STAMP *pts, short *st, short *se)
{
unsigned idx,i;

	while ( !isdigit(*name) && *name )
			name++;

	if (1!=sscanf(name,"%i",&idx) || idx >3) {
		printf("Invalid index\n");
		return -1;
	}
	if ( nelms < 0 || nelms > 3 ) {
		printf("Invalid nelms\n");
		return -1;
	}
	for (i=0; i<nelms; i++)
	switch (type) {
		case ezcaShort:  ((short*)bufp)[i] =tstSht[3*idx+i]; break;
		case ezcaLong:   ((long*)bufp)[i]  =tstLng[3*idx+i]; break;
		case ezcaFloat:  ((float*)bufp)[i] =tstFlt[3*idx+i]; break;
		case ezcaDouble: ((double*)bufp)[i]=tstDbl[3*idx+i]; break;
		case ezcaString: strcpy(&((dbr_string_t*)bufp)[i][0], tstChr[3*idx+i] ? tstChr[3*idx+i]:""); break;

		default: printf("Invalid type\n"); return -1;

	}
	*st = *se = 0;
	return 0;
}

#endif


/* scilab external type converters */

void C2F(cshortf)(int *n, short *ip[], double *op)
{
int i;
	for ( i=0; i<*n; i++ )
		op[i]=(double)(*ip)[i];
	free((void*)*ip);
}

/* extra hacking - we store time into a complex number.
 * Scilab stores those into two separate arrays and hence this
 * routine is called twice; one for the real part and one more
 * time for the imaginary. Hence, we must keep some state information
 * to do the proper bookkeeping :-(
 */

typedef struct TimeArgRec_ {
	TS_STAMP	*pts;	/* the array of timestamps */
	char		doFree;	/* free timestamp array after use */
	char		imag;   /* called for the real or imaginary part? */
} TimeArgRec, *TimeArg;

static int timeArgsAlloc(TimeArg *pre, TimeArg *pim, int *phasImag)
{
int		rval;
TimeArg re = 0, im = 0;

	*pre = *pim = 0;

	*phasImag = 1;

	if ( rval =  !( re = calloc( 1, sizeof(*re) )) || (*phasImag && !( im = calloc( 1, sizeof( *im ) ) )) ) {
		cerro("timeArgsAlloc: no memory");
		goto cleanup;
	}

	if ( *phasImag ) {
		im->doFree = 1; /* last call is for the imaginary part */
		im->imag   = 1;
	} else {
		re->doFree = 1;
	}

	*pre = re; re = 0;
	*pim = im; im = 0;

cleanup:
	free(re);
	free(im);

	return rval;
}

static int timeArgsRelease(TimeArg *pre, TimeArg *pim, int *phasImag)
{
	free(*pre); *pre = 0;
	free(*pim); *pim = 0;
	*phasImag = 0;
}

void C2F(cts_stampf_)(int *n, TimeArg *ip, double *op)
{
int i;
struct timespec ts;
	for ( i=0; i<*n; i++ ) {
		epicsTimeToTimespec( &ts, (*ip)->pts + i );
		op[i]=(double) ((*ip)->imag ? ts.tv_nsec : ts.tv_sec);
	}
	if ( (*ip)->doFree )
		free( (*ip)->pts );
	free((void*)*ip);
}

static void
ezErr(char *nm)
{
	char *msg;
	ezcaGetErrorString(nm,&msg);
	if (msg) cerro(msg);
	ezcaFree(msg);
}

/* determine the size of an ezcaXXX type */
static int typesize(int type)
{
	switch (type) {
		case ezcaShort:		return sizeof(short);
		case ezcaLong:		return sizeof(long);
		case ezcaFloat:		return sizeof(float);
		case ezcaDouble:	return sizeof(double);
		case ezcaString:	return sizeof(dbr_string_t);
		default: break;
	}
	assert(!"must never get here");
	return -1;
}

/* transpose and convert a matrix */
#define XPOSECVT(Forttyp,check,Ctyp,assign) \
	{ Ctyp *cpt; Forttyp *fpt; \
	for ( i=0, cpt=cbuf; i<m; i++ ) { \
		for ( j=0, fpt=(Forttyp*)fbuf+i; j<n; j++, fpt+=m) { \
			if ( check ) { \
				cpt += n-j; \
				break; \
			} else { \
				assign; \
				cpt++; \
			} \
		} \
		dims[i] = j; \
	} \
	}

static void
multi_ezca_put(char **nms, char type, void *fbuf, int m, int n)
{
void          *cbuf = 0;
int           *dims = 0;
int           rowsize;

register int  i,j;
register char *bufp;

	/* get buffers; since we do asynchronous processing, we
	 * need to buffer the full array - we cannot do it row-wise
	 */

	rowsize = n * typesize(type);

	if ( !(cbuf = malloc( m * rowsize )) || !(dims = malloc(sizeof(*dims) * m)) ) {
		cerro("multi_ezca_put: not enough memory");
		goto cleanup;
	}

	/* transpose and convert */
	switch ( type ) {
		case ezcaShort:   XPOSECVT( double, isnan(*fpt), short,  *cpt=*fpt ); break;
		case ezcaLong :   XPOSECVT( double, isnan(*fpt), long,   *cpt=*fpt ); break;
		case ezcaFloat:   XPOSECVT( double, isnan(*fpt), float,  *cpt=*fpt ); break;
		case ezcaDouble:  XPOSECVT( double, isnan(*fpt), double, *cpt=*fpt ); break;
		case ezcaString:  XPOSECVT( char*,
								    (!*fpt || !**fpt),
									dbr_string_t,
									if ( strlen(*fpt) >= sizeof(dbr_string_t) ) { \
									    cerro("string too long");\
										goto cleanup; \
									} else \
										strcpy(&(*cpt)[0],*fpt)
									);
						  break;
	}

	ezcaStartGroup();

		for ( i=0, bufp = cbuf; i<m; i++, bufp+=rowsize ) {
			ezcaPut(nms[i], type, dims[i], bufp);
		}

	if (ezcaEndGroup())
		ezErr("multi_ezca_put");

cleanup:
	free(cbuf);
	free(dims);
}

static int
multi_ezca_get(char **nms, char type, void **pres, int m, int *pn, TimeArg *pre, TimeArg *pim, int *hasImag)
{
void          *cbuf = 0;
void          *fbuf = 0;
int           *dims = 0;
short         *stat = 0;
short         *sevr = 0;
int           rval  = 0;
int           rowsize;
int tsill=0;

TS_STAMP      *ts   = 0;

register int  i,j,n;
register char *bufp;

	*pn   = 0;
	*pres = 0;
	*pre = *pim = 0;
	*hasImag = 0;

	/* get buffers; since we do asynchronous processing, we
	 * need to buffer the full array - we cannot do it row-wise
	 */
	if ( !(dims = calloc( m , sizeof(*dims) )) ) {
		cerro("multi_ezca_get: not enough memory");
		goto cleanup;
	}

	/* obtain target array sizes */
	/* EZCA Bug: ezcaGetNelem() doesn't work in grouped mode :-( */
#if EZCA_BUG_FIXED
	ezcaStartGroup();
		for ( i=0; i<m; i++)
			ezcaGetNelem( nms[i], dims+i );
	if ( ezcaEndGroup() ) {
		ezErr("ezcaGetNelem");
		goto cleanup;
	}
#else
	for ( i=0; i<m; i++) {
		if ( ezcaGetNelem( nms[i], dims+i ) ) {
			ezErr("ezcaGetNelem");
			goto cleanup;
		}
	}
#endif

	for ( n=i=0; i<m; i++) {
		if ( dims[i] > n ) n = dims[i];
	}


	rowsize = n * typesize(type);

	if ( !(cbuf = malloc( m * rowsize ))          ||
		 !(stat = calloc( m,  sizeof(*stat)))     ||
		 !(ts   = malloc( m * sizeof(TS_STAMP)))  ||
		 !(sevr = malloc( m * sizeof(*sevr)))     ||
		 timeArgsAlloc(pre, pim, hasImag) ) {
		cerro("multi_ezca_get: not enough memory");
		goto cleanup;
	}

	/* get the values along with status */
	ezcaStartGroup();
		for ( i=0, bufp=cbuf; i<m; i++, bufp+=rowsize ) {
			if ( ezcaGetWithStatus(nms[i],type,dims[i], bufp,ts + i,stat+i,sevr+i) ) {
				ezErr("multi_ezca_get");
				goto cleanup;
			}
		}
#ifdef SILENT_AND_PROGRESS
	{
	int *rcs = 0;
	int nrcs;
	ezcaEndGroupWithReport(&rcs, &nrcs);
	assert(nrcs == m);
	for ( i=0; i<nrcs; i++ )
		if ( EZCA_OK != rcs[i] )
			dims[i] = 0;
	ezcaFree(rcs);
	}
#else
	if ( ezcaEndGroup() ) {
		ezErr("multi_ezca_get");
		goto cleanup;
	}
#endif

	for ( i=0; i<m; i++ ) {
		if ( sevr[i] >= ezcaSeverityWarnLevel )
			mexPrintf("Warning: PV (%s) with alarm status: %s (severity %s)\n",
						nms[i],
						alarmStatusString[stat[i]],
						alarmSeverityString[sevr[i]]);
	}

	/* allocate the target buffer */
	if ( ezcaString == type )
		fbuf = calloc( m*n, sizeof(char*) );
	else
		fbuf = malloc( m*n * sizeof(double) );

	if ( !fbuf ) {
		cerro("multi_ezca_get: no memory");
		goto cleanup;
	}

	/* transpose and convert */
	switch ( type ) {
			case ezcaShort:   XPOSECVT( double,
										(0),
										short,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaLong :   XPOSECVT( double,
										(0),
										long,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaFloat:   XPOSECVT( double,
										(0),
										float,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaDouble:  XPOSECVT( double,
										(0),
										double,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaString:  XPOSECVT( char *,
										(0),
										dbr_string_t,
										do {  \
										  if ( !(*fpt=strdup(j>=dims[i] ? \
																"" :      \
															    ( INVALID_ALARM == sevr[i] ?  \
																    "<INVALID_ALARM>" :       \
																	&(*cpt)[0]                \
															    )         \
															 ) \
												) ) {  \
										     cerro("strdup: no memory"); \
											 goto cleanup; \
										  } \
										} while (0)
							  );
							  break;
	}

	*pres = fbuf; fbuf  = 0;
	(*pre)->pts = (*pim)->pts = ts; ts = 0;

	*pn   = n;
	rval  = m;

cleanup:
	if ( fbuf && ezcaString == type ) {
		for ( i=0; i<n*m; i++)
			free(((char**)fbuf)[i]);
	}
	free(fbuf);
	free(cbuf);
	free(dims);
	free(stat);
	free(sevr);
	if ( ts ) {
		free(ts);
		timeArgsRelease(pre,pim,hasImag);
	}
	return rval;
}

typedef struct MultiArgRec_ {
	int		size;
	void	*buf;
	void	**pres;
} MultiArgRec, *MultiArg;

static int multi_ezca_get_misc(char **nms, int m, int (*ezcaProc)(), int nargs, MultiArg args)
{
int		rval = 0;
int		i,sz;
char	*bufp0;
char	*bufp1;
char	*bufp2;
char	*bufp3;

	/* allocate buffers */
	for ( i=0; i<nargs; i++ ) {
		if ( args[i].pres )
			*args[i].pres = 0;
		if ( args[i].buf )
				continue; /* caller allocated the buffer */

		if ( !(args[i].buf = calloc( m, args[i].size )) ) {
			cerro("no memory");
			goto cleanup;
		}
	}

	ezcaStartGroup();

	switch (nargs) {
		case 1:
		for ( i=0,
			  bufp0=args[0].buf
			  ;
			  i<m;
			  i++,
			  bufp0+=args[0].size
			  )
			ezcaProc(nms[i],bufp0);
		break;

		case 2:
		for ( i=0,
			  bufp0=args[0].buf,
			  bufp1=args[1].buf
			  ;
			  i<m;
			  i++,
			  bufp0+=args[0].size,
			  bufp1+=args[1].size
			  )
			ezcaProc(nms[i],bufp0,bufp1);
		break;

		case 3:
		for ( i=0,
			  bufp0=args[0].buf,
			  bufp1=args[1].buf,
			  bufp2=args[2].buf
			  ;
			  i<m;
			  i++,
			  bufp0+=args[0].size,
			  bufp1+=args[1].size,
			  bufp2+=args[2].size
			  )
			ezcaProc(nms[i],bufp0,bufp1,bufp2);
		break;

		case 4:
		for ( i=0,
			  bufp0=args[0].buf,
			  bufp1=args[1].buf,
			  bufp2=args[2].buf,
			  bufp3=args[3].buf
			  ;
			  i<m;
			  i++,
			  bufp0+=args[0].size,
			  bufp1+=args[1].size,
			  bufp2+=args[2].size,
			  bufp3+=args[3].size
			  )
			ezcaProc(nms[i],bufp0,bufp1,bufp2,bufp3);
		break;
	default:
		assert(!"multi_ezca_get_misc: invalid number of arguments - should never happen");
	}

	if ( ezcaEndGroup() ) {
		ezErr("multi_ezca_get_misc");
		goto cleanup;
	}

	rval  = m;
	for ( i=0; i<nargs; i++ ) {
		if ( args[i].pres )
				*args[i].pres = args[i].buf;
	}
	nargs = 0;

cleanup:
	for ( i=0; i<nargs; i++ )
		free (args[i].buf);
	return rval;
}

void
C2F(ezget)(char ***nms, int *m, char *type, double **buf, int *mo, int *n, TimeArg *pre, TimeArg *pim, int *hasImag, int *one)
{
char typenum = ezcaDouble;

	*n   = 0;
	*buf = 0;
	*mo  = 0;

	*one = 1;

	/* strange quirk: sometimes intersci doesn't get dimensions right if
	 *                we try to use a single scilab variable for 'm' and 'mo'
	 *                in the 'desc' file...
	 */


	switch ( toupper(type[0]) ) {
		case 'D': typenum = ezcaDouble; break;
		case 'F': typenum = ezcaFloat;  break;
		case 'S': typenum = ezcaShort;  break;
		case 'L': typenum = ezcaLong;   break;

		default: 
			cerro("Invalid type - must be 'short', 'long', 'float' or 'double'");
		return;
	}

	*mo = multi_ezca_get(*nms, typenum, (void**)buf, *m, n, pre, pim, hasImag);
}

void
C2F(ezgetstring)(char ***nms, int *m, char ***buf, int *mo, int *n, TimeArg *pre, TimeArg *pim, int *hasImag, int *one)
{
	*one = 1;
	*mo = multi_ezca_get(*nms, ezcaString, (void**)buf, *m, n, pre, pim, hasImag);
}


void
C2F(ezput)(char ***nms, int *m, char *type, double *val, int *n)
{
char typenum;

	switch ( toupper(type[0]) ) {
		case 'S':	typenum = ezcaShort;  break;
		case 'L':	typenum = ezcaLong;   break;
		case 'F':	typenum = ezcaFloat;  break;
		case 'D':	typenum = ezcaDouble; break;

		default:
			cerro("Invalid type - must be 'short', 'long', 'float' or 'double'");
		return;
	}

	multi_ezca_put(*nms, typenum, val, *m, *n);
}

void
C2F(ezputstring)(char ***nms, int *m, char ***buf, int *n)
{
	multi_ezca_put(*nms, ezcaString, *buf, *m, *n);
}

void
C2F(ezgetnelem)(char ***nms, int *m, int **pnelms, int *mo)
{
int i;
int *nelms = 0;

	*mo     = 0;
	*pnelms = 0;
	if ( ! (nelms = calloc( *m, sizeof(*nelms) )) ) {
		cerro("no memory");	
		goto cleanup;
	}
	/* ezcaGetNelem() doesn't work in grouped mode :-( */
	for ( i=0; i<*m; i++) {
		if ( ezcaGetNelem((*nms)[i],nelms+i) ) {
			ezErr("ezgetnelem");
			goto cleanup;
		}
	}
	*pnelms = nelms; nelms = 0;
	*mo     = *m;

cleanup:
	free(nelms);
}

void
C2F(ezgetcontrollimits)(char ***nms, int *m, double **plo, double **phi, int *mo)
{
MultiArgRec args[]={ { sizeof(double), 0, (void**)plo }, { sizeof(double), 0, (void**)phi } };

    *mo = multi_ezca_get_misc(*nms, *m, ezcaGetControlLimits, NumberOf(args), args);

}

void
C2F(ezgetgraphiclimits)(char ***nms, int *m, double **plo, double **phi, int *mo)
{
MultiArgRec args[]={ { sizeof(double), 0, (void**)plo }, { sizeof(double), 0, (void**)phi } };
    *mo = multi_ezca_get_misc(*nms, *m, ezcaGetGraphicLimits, NumberOf(args), args);
}

void
C2F(ezgetstatus)(char ***nms, int *m, TimeArg *pre, TimeArg *pim, int *hasImag, int *one, short **sta, short **svr, int *mo)
{
TS_STAMP *ts;
MultiArgRec args[]={
		{ sizeof(TS_STAMP), 0, (void**)&ts },
		{ sizeof(short),    0, (void**)sta },
		{ sizeof(short),    0, (void**)svr },
		};

    *one = 1;

	if ( timeArgsAlloc(pre,pim,hasImag) )
		return;

	if ( *mo = multi_ezca_get_misc(*nms, *m, ezcaGetStatus, NumberOf(args), args) )
		(*pre)->pts = (*pim)->pts = ts;
	else
		timeArgsRelease(pre,pim,hasImag);
}

void
C2F(ezgetprecision)(char ***nms, int *m, short **prec, int *mo)
{
MultiArgRec args[]={
		{ sizeof(short),    0, (void**)prec },
		};

	*mo = multi_ezca_get_misc(*nms, *m, ezcaGetPrecision, NumberOf(args), args);
}

typedef char units_string[EZCA_UNITS_SIZE];

void
C2F(ezgetunits)(char ***nms, int *m, char ***units, int *mo)
{
int i,dim;
MultiArgRec args[]={
		{ sizeof(units_string), 0, (void**)0 },
		};
char **buf = 0;

	*units = 0;
	*mo    = 0;

	if ( dim = multi_ezca_get_misc(*nms, *m, ezcaGetUnits, NumberOf(args), args) ) {
		if ( !(buf = calloc( *m, sizeof(*buf) )) ) {
			cerro("no memory");
			goto cleanup;
		}

		for ( i=0; i<*m; i++ )
			if ( !(buf[i] = strdup( &((units_string*)args[0].buf)[i][0] )) ) {
				cerro("no memory");
				goto cleanup;
			}

		*units = buf; buf = 0;
		*mo    = dim; 
	}

cleanup:
	if ( buf ) {
		for ( i=0; i<*m; i++)
			free(buf[i]);
		free(buf);
	}
	free(args[0].buf);
}

void
C2F(ezgetretrycount)(int *count)
{
	*count = ezcaGetRetryCount();
}

void
C2F(ezsetretrycount)(int *count)
{
	ezcaSetRetryCount(*count);
}

void
C2F(ezgettimeout)(float *tout)
{
	*tout = ezcaGetTimeout();
}

void
C2F(ezsettimeout)(float *tout)
{
	ezcaSetTimeout(*tout);
}

void
C2F(ezdebugon)(void)
{
	ezcaDebugOn();
}

void
C2F(ezdebugoff)(void)
{
	ezcaDebugOff();
}

void
C2F(ezsetseveritywarnlevel)(int *level)
{
	ezcaSeverityWarnLevel = *level;
}

void
C2F(ezlibinit)(void)
{
/* don't print to stderr because that
 * doesn't go to scilab's main window...
 */
ezcaAutoErrorMessageOff();
ezcaSetTimeout(0.1);
ezcaSetRetryCount(40);
}
