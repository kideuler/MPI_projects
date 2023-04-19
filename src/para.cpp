#include "MPIsort.hpp"

void Init_parallel(){
    #ifdef USE_MPI
    MPI_Init(NULL,NULL);
    #endif
    return;
}

void Finalize_parallel(){
    #ifdef USE_MPI
    MPI_Finalize();
    #endif
    return;
}

int get_rank(){
    int rank = 0;
    #ifdef USE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif
    return rank;
}