#ifndef LCA_ERROR_H
#define LCA_ERROR_H
/* $Id$ */

/* labca error interface */

typedef struct LcaError_ {
	int  err;
	char msg[100];
} LcaError;

/* Errors returned by ezcaNewMonitorValue */

#define EZCA_NOMONITOR	20
#define EZCA_NOCHANNEL	21

#ifdef __cplusplus
extern "C" {
#endif

#define lcaErrorInit(pe)	do { (pe)->err = 0; (pe)->msg[0] = 0; } while (0)

#ifdef SCILAB_APP
#define LCA_RAISE_ERROR(theError)	do { if ( (theError)->err ) { Scierror((theError)->err + 10000, (theError)->msg); } } while (0)
#endif

#ifdef __cplusplus
};
#endif

#endif
