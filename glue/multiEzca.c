/* $Id: multiEzca.c,v 1.1 2003/12/11 05:33:08 till Exp $ */

/* multi-PV EZCA calls */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#ifndef MACHHACK
#include <mex.h> /* fortran/C name conversion for scilab */
#else
#define C2F(name) name##_
#endif

#include "multiEzca.h"

/* CA includes */
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>
#include <alarm.h>
#include <alarmString.h>


/* GLOBAL VARIABLES */
int ezcaSeverityWarnLevel = INVALID_ALARM;

#undef TESTING

/* ezca needs a bugfix for correctly reading Nelem in grouped mode */

#define EZCA_BUG_FIXED 1 /* patch applied */

#if EZCA_BUG_FIXED
#define EZCA_START_NELEM_GROUP()	ezcaStartGroup()
#define EZCA_END_NELEM_GROUP()		ezcaEndGroup()
#else
/* EZCA Bug: ezcaGetNelem() doesn't work in grouped mode :-( */
#define EZCA_START_NELEM_GROUP()	(0)
#define EZCA_END_NELEM_GROUP()		(0)
#endif



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
				case ezcaByte:   printf(" %i", ((char*)bufp)[j]); break;
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
		case ezcaByte:   ((char*)bufp)[i]  =tstSht[3*idx+i]; break;
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

/* our 'strdup' implementation (*IMPORTANT*, in order for the matlab implementation using mxMalloc!!)
 * also, strdup is no POSIX...
 */
char *
my_strdup(const char *str)
{
char *rval = 0;
	/* under matlab, 'malloc' will be replaced by mxMalloc by means of macro uglyness */ 
	if ( str && (rval = malloc(sizeof(*rval) * (strlen(str) + 1) )) ) {
		strcpy(rval, str);
	}
	return rval;
}


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

int
timeArgsAlloc(TimeArg *pre, TimeArg *pim, int *phasImag)
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

int
timeArgsRelease(TimeArg *pre, TimeArg *pim, int *phasImag)
{
	if ( (*pre) ) {
		free( (*pre)->pts );
		free(*pre); *pre = 0;
	}
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
	*ip = 0;
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
static int typesize(char type)
{
	switch (type) {
		case ezcaByte:		return sizeof(char);
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

static char nativeNumType(char *pv)
{
chid *pid;

	if ( EZCA_OK == ezcaPvToChid( pv, &pid ) && pid ) {
		switch( ca_field_type(*pid) ) {
			default:	/* includes TYPE_NOTCONN */
				             break;

			case DBF_CHAR:   return ezcaByte;

			/*	case DBF_INT: */
			case DBF_SHORT:  return ezcaShort;

			case DBF_LONG:   return ezcaLong;
			case DBF_FLOAT:  return ezcaFloat;
			case DBF_DOUBLE: return ezcaDouble;
		}
	}
	return ezcaFloat;
}

int
multi_ezca_get_nelem(char **nms, int m, int *dims)
{
int i;

	EZCA_START_NELEM_GROUP();

		for ( i=0; i<m; i++) {
			if ( ezcaGetNelem( nms[i], dims+i ) ) {
				ezErr("ezcaGetNelem");
				return -1;
			}
		}

	if ( EZCA_END_NELEM_GROUP() ) {
		ezErr("ezcaGetNelem");
		return -1;
	}

	return 0;
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

/* for ( i=0, bufp = cbuf; i<m; i++, bufp+=rowsize) */
#define CVTVEC(Forttyp, check, Ctyp, assign)			\
	{ Ctyp *cpt = (Ctyp*)bufp; Forttyp *fpt = (Forttyp*)fbuf + (mo > 1 ? i : 0);	\
		for ( j = 0; j<n && !(check) ; j++, cpt++, fpt+=mo ) { \
			assign;	\
		}	\
	}

void
multi_ezca_put(char **nms, int m, char type, void *fbuf, int mo, int n)
{
void          *cbuf  = 0;
int           *dims  = 0;
char		  *types = 0;
int           rowsize,typesz;

register int  i,j;
register char *bufp;

	if ( mo != 1 && mo != m ) {
		cerro("multi_ezca_put: invalid dimension of 'value' matrix; must have 1 or m rows");
		goto cleanup;
	}

	if ( !(dims  = malloc( m * sizeof(*dims) )) ||
	     !(types = malloc( m * sizeof(*types) )) ) {
		cerro("multi_ezca_put: not enough memory");
		goto cleanup;
	}


#if EZCA_BUG_FIXED
	/* PvToChid is non-groupable; hence we request all 'nelms'
     * hoping that batching ca connects is faster than looping
     * through a number of PvToChids
     */
	if ( ezcaNative == type ) {
		if ( multi_ezca_get_nelem( nms, m, dims ) )
			goto cleanup;
	}
#endif

	typesz = 0;
	for ( i=0; i<m; i++ ) {
		int tmp;
		types[i] = ezcaNative == type ? nativeNumType(nms[i]) : type;
		if ( (tmp = typesize(types[i])) > typesz ) {
			typesz = tmp;
		}
	}

	rowsize = n * typesz;

	/* get buffers; since we do asynchronous processing and we possibly
     * use different native types for different rows, we
	 * need to buffer the full array - we cannot do it row-wise
	 */

	if ( !(cbuf  = malloc( m * rowsize )) ) {
		cerro("multi_ezca_put: not enough memory");
		goto cleanup;
	}

	/* transpose and convert */
	for ( i=0, bufp = cbuf; i<m; i++, bufp+=rowsize) {
	switch ( types[i] ) {
		case ezcaByte:    CVTVEC( double, isnan(*(double*)fpt), char,   *cpt=*fpt ); break;
		case ezcaShort:   CVTVEC( double, isnan(*(double*)fpt), short,  *cpt=*fpt ); break;
		case ezcaLong :   CVTVEC( double, isnan(*(double*)fpt), long,   *cpt=*fpt ); break;
		case ezcaFloat:   CVTVEC( double, isnan(*(double*)fpt), float,  *cpt=*fpt ); break;
		case ezcaDouble:  CVTVEC( double, isnan(*(double*)fpt), double, *cpt=*fpt ); break;
		case ezcaString:  CVTVEC( char*,
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
	dims[i] = j;
	}

	ezcaStartGroup();

		for ( i=0, bufp = cbuf; i<m; i++, bufp += rowsize ) {
			ezcaPut(nms[i], types[i], dims[i], bufp);
		}

	if (ezcaEndGroup())
		ezErr("multi_ezca_put");

cleanup:
	free(types);
	free(cbuf);
	free(dims);
}

int
multi_ezca_get(char **nms, char type, void **pres, int m, int *pn, TimeArg *pre, TimeArg *pim, int *hasImag)
{
void          *cbuf  = 0;
void          *fbuf  = 0;
int           *dims  = 0;
short         *stat  = 0;
short         *sevr  = 0;
int           rval   = 0;
char          *types = 0;
TS_STAMP      *ts    = 0;
int           rowsize,typesz,nreq;
int           mo     = m;
chid		  *pid;

register int  i,j,n;
register char *bufp;

	nreq  = *pn;

	*pn   = 0;
	*pres = 0;
	*pre = *pim = 0;
	*hasImag = 0;

	/* get buffers; since we do asynchronous processing, we
	 * need to buffer the full array - we cannot do it row-wise
	 */
	if ( !(dims = calloc( m , sizeof(*dims) )) || !(types = malloc( m * sizeof(*types) )) ) {
		cerro("multi_ezca_get: not enough memory");
		goto cleanup;
	}

	if ( multi_ezca_get_nelem( nms, m, dims ) )
		goto cleanup;

	typesz = 0;
	for ( n=i=0; i<m; i++) {
		int tmp;

		/* clip to requested n */
		if ( nreq > 0 && dims[i] > nreq )
			dims[i] = nreq;

		if ( dims[i] > n )
			n = dims[i];
		/* nativeNumType uses ezcaPvToChid() which is non-groupable */
		types[i] = ezcaNative == type ? nativeNumType( nms[i] ) : type;

		if ( (tmp = typesize(types[i])) > typesz )
			typesz = tmp;
	}

	rowsize = n * typesz;

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
			if ( ezcaGetWithStatus(nms[i],types[i],dims[i], bufp,ts + i,stat+i,sevr+i) ) {
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
	for ( i=0, bufp = cbuf; i<m; i++, bufp+=rowsize) {
	switch ( types[i] ) {
			case ezcaByte:   CVTVEC( double,
										(0),
										char,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaShort:   CVTVEC( double,
										(0),
										short,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaLong :   CVTVEC( double,
										(0),
										long,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaFloat:   CVTVEC( double,
										(0),
										float,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaDouble:  CVTVEC( double,
										(0),
										double,
										*fpt= j>=dims[i] || INVALID_ALARM == sevr[i] ? NAN : *cpt
							  );
							  break;

			case ezcaString:  CVTVEC( char *,
										(0),
										dbr_string_t,
										do {  \
										  if ( !(*fpt=my_strdup(j>=dims[i] ? \
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
	dims[i] = j;
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
	free(types);
	free(stat);
	free(sevr);
	if ( ts ) {
		timeArgsRelease(pre,pim,hasImag);
	}
	return rval;
}

int
multi_ezca_get_misc(char **nms, int m, int (*ezcaProc)(), int nargs, MultiArg args)
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
