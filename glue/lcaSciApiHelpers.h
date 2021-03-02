#ifndef LCA_SCI_API_HELPERS_H
#define LCA_SCI_API_HELPERS_H

#include <api_scilab.h>
#include <lcaError.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *PvApiCtxType;

int
lcaCheckSciError(LcaError *pe, SciErr *sciErr);

char **
lcaGetApiStringMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

void *
lcaGetApiPtrMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np, int *type);

int *
lcaGetApiIntMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

double *
lcaGetApiDblMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

void
lcaFreeApiStringMatrix(char **sm);

#ifdef __cplusplus
};
#endif

#endif
