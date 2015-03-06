#ifndef SPLATT_MPI_H
#define SPLATT_MPI_H

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "base.h"
#include "sptensor.h"
#include "reorder.h"
#include "cpd.h"
#include <mpi.h>


/******************************************************************************
 * PUBLIC STRUCTURES
 *****************************************************************************/

/**
* @brief A structure for MPI rank structures (communicators, etc.).
*/
typedef struct
{
  /* Tensor information */
  idx_t global_nnz;
  idx_t global_dims[MAX_NMODES];
  idx_t mat_start[MAX_NMODES];
  idx_t mat_end[MAX_NMODES];
  idx_t layer_starts[MAX_NMODES];
  idx_t layer_ends[MAX_NMODES];

  /* start/end idxs for each process */
  idx_t * mat_ptrs[MAX_NMODES];

  /* MPI_Alltoallv info */
  idx_t sends[MAX_NMODES];
  idx_t recvs[MAX_NMODES];
  idx_t * nbrind[MAX_NMODES];   /** inds that I compute for but others own */
  idx_t * localind[MAX_NMODES]; /** inds that others compute for but I own */
  int   * nbrptr[MAX_NMODES];
  int   * nbrdisp[MAX_NMODES];
  int   * localptr[MAX_NMODES];
  int   * localdisp[MAX_NMODES];
  idx_t * nbrmap[MAX_NMODES];   /** map nbrind into my local coord space */

  /* Communicators */
  MPI_Comm comm_3d;
  MPI_Comm layer_comm[MAX_NMODES];

  /* Rank information */
  int rank;
  int npes;
  int np13; /* cube root of npes */
  int rank_3d;
  int mode_rank[MAX_NMODES];
  int dims_3d[MAX_NMODES];
  int coords_3d[MAX_NMODES];
  int layer_rank[MAX_NMODES];


  /* Miscellaneous */
  MPI_Status status;
  MPI_Request req;
  idx_t worksize;
} rank_info;


/******************************************************************************
 * PUBLIC FUNCTONS
 *****************************************************************************/


/**
* @brief
*
* @param tt
* @param mats
* @param globmats
* @param rinfo
* @param opts
*/
void mpi_cpd(
  sptensor_t * const tt,
  matrix_t ** mats,
  matrix_t ** globmats,
  rank_info * const rinfo,
  cpd_opts const * const opts);


/**
* @brief
*
* @param rinfo
* @param tt
*/
void mpi_compute_ineed(
  rank_info * const rinfo,
  sptensor_t const * const tt,
  idx_t const nfactors);

/**
* @brief Each rank reads their 3D partition of a tensor.
*
* @param ifname The file containing the tensor.
* @param rinfo Rank information, assumes mpi_setup_comms() has been called
*              first!
*
* @return The rank's subtensor.
*/
sptensor_t * mpi_tt_read(
  char const * const ifname,
  rank_info * const rinfo);


/**
* @brief Compute a distribution of factor matrices that minimizes communication
*        volume.
*
* @param rinfo MPI structure containing rank and communicator information.
* @param tt A partition of the tensor. NOTE: indices will be reordered after
*           distribution to ensure contiguous matrix partitions.
*
* @return The permutation that was applied to tt.
*/
permutation_t *  mpi_distribute_mats(
  rank_info * const rinfo,
  sptensor_t * const tt);

/**
* @brief Fill rinfo with process' MPI rank information. Includes rank, 3D
*        communicator, etc.
*
* @param rinfo The rank data structure.
*/
void mpi_setup_comms(
  rank_info * const rinfo);


/**
* @brief Free structures allocated inside rank_info.
*
* @param rinfo The rank structure to free.
* @param nmodes The number of modes that have been allocated.
*/
void rank_free(
  rank_info rinfo,
  idx_t const nmodes);


/**
* @brief Print send/recieve information to STDOUT.
*
* @param rinfo MPI rank information. Assumes mpi_distribute_mats() has already
*              been called.
* @param tt The distributed tensor.
*/
void mpi_send_recv_stats(
  rank_info const * const rinfo,
  sptensor_t const * const tt);

#endif
