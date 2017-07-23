#include <stdio.h>

#include "omp.h"
#include "mpi.h"

int main(int argc, char* argv[]) {
  int procSize,thrSize=5;
  int tid,pid;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &procSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  omp_set_num_threads(thrSize);
#pragma omp parallel private(tid)
  {
  tid=omp_get_thread_num();
  printf("Hello from proc[%d] thread[%d]\n",pid,tid);
  }

  MPI_Finalize();
  return 0;
}

