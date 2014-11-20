

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "base.h"
#include "ftensor.h"
#include "sort.h"


/******************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/
static void __create_fptr(
  ftensor_t * const ft,
  sptensor_t const * const tt,
  idx_t const mode)
{
  if(tt->type == SPLATT_NMODE) {
    fprintf(stderr, "SPLATT: ftensor for n-mode tensors is finished.\n");
    exit(1);
  }

  idx_t const nnz = tt->nnz;
  idx_t const nmodes = tt->nmodes;
  idx_t const fmode = ft->dim_perms[mode][nmodes - 1];

  idx_t nfibs = 1;
  ft->inds[mode][0] = tt->ind[fmode][0];
  ft->vals[mode][0] = tt->vals[0];

  /* count fibers in tt */
  for(idx_t n=1; n < nnz; ++n) {
    int newfib = 0;
    /* check for new fiber */
    for(idx_t m=0; m < nmodes-1; ++m) {
      if(tt->ind[m][n] != tt->ind[m][n-1]) {
        newfib = 1;
        break;
      }
    }
    if(newfib) {
      ++nfibs;
    }
    ft->inds[mode][n] = tt->ind[fmode][n];
    ft->vals[mode][n] = tt->vals[n];
  }

  /* allocate fiber structure */
  ft->nfibs[mode] = nfibs;
  ft->fptr[mode] = (idx_t *) malloc((nfibs+1) * sizeof(idx_t));
  ft->fids[mode] = (idx_t *) malloc(nfibs * sizeof(idx_t));

  /* now fill structure */
  ft->fptr[mode][0] = 0;
  ft->fids[mode][0] = tt->ind[mode][ft->dim_perms[mode][1]];
}

static void __create_sptr(
  ftensor_t * const ft,
  sptensor_t const * const tt,
  idx_t const mode)
{
  ft->sptr[mode] = (idx_t *) malloc((ft->dims[mode]+1) * sizeof(idx_t));
}

static void __order_modes(
  ftensor_t * const ft,
  sptensor_t const * const tt)
{
  for(idx_t m=0; m < tt->nmodes; ++m) {
    ft->dim_perms[m][0] = m;
    /* find largest mode */
    idx_t maxm = (m+1) % tt->nmodes;
    for(idx_t mo=1; mo < tt->nmodes; ++mo) {
      if(ft->dims[(m+mo) % tt->nmodes] > ft->dims[maxm]) {
        maxm = (m+mo) % tt->nmodes;
      }
    }

    /* fill in mode permutation */
    ft->dim_perms[m][tt->nmodes-1] = maxm;
    idx_t mark = 1;
    for(idx_t mo=1; mo < tt->nmodes; ++mo) {
      idx_t mround = (m + mo) % tt->nmodes;
      if(mround != maxm) {
        ft->dim_perms[m][mark++] = mround;
      }
    }
  }
}


/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/
ftensor_t * ften_alloc(
  sptensor_t * const tt)
{
  ftensor_t * ft = (ftensor_t *) malloc(sizeof(ftensor_t));
  ft->nnz = tt->nnz;
  ft->nmodes = tt->nmodes;

  for(idx_t m=0; m < tt->nmodes; ++m) {
    ft->dims[m] = tt->dims[m];
  }

  /* compute permutation of modes */
  __order_modes(ft, tt);

  /* allocate modal data */
  for(idx_t m=0; m < tt->nmodes; ++m) {
    ft->inds[m] = (idx_t *) malloc(ft->nnz * sizeof(idx_t));
    ft->vals[m] = (val_t *) malloc(ft->nnz * sizeof(val_t));

    tt_sort(tt, m, NULL);
    __create_fptr(ft, tt, m);
    __create_sptr(ft, tt, m);
  }

  return ft;
}

void ften_free(
  ftensor_t * ft)
{
  for(idx_t m=0; m < ft->nmodes; ++m) {
    free(ft->sptr[m]);
    free(ft->fptr[m]);
    free(ft->fids[m]);
    free(ft->inds[m]);
    free(ft->vals[m]);
  }
  free(ft);
}


