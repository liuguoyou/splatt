#ifndef SPLATT_FTENSOR_H
#define SPLATT_FTENSOR_H

#include "base.h"
#include "sptensor.h"

typedef struct
{
  idx_t nnz;
  idx_t nmodes;
  idx_t dims[MAX_NMODES];
  idx_t dim_perms[MAX_NMODES][MAX_NMODES];

  idx_t * sptr[MAX_NMODES];
  idx_t * fptr[MAX_NMODES];
  idx_t * fids[MAX_NMODES];
  idx_t * inds[MAX_NMODES];
  val_t * vals[MAX_NMODES];
} ftensor_t;

ftensor_t * ften_alloc(
  sptensor_t * const tt);

void ften_free(
  ftensor_t * ft);

#endif