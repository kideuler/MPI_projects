#include "MPIsort.hpp"


int main(int argc, char *argv[]){
    int n = 201;
    Init_parallel();
    int rank = get_rank();
    ArraySort<double> X;
    if (rank == 0){
    double x[n];
    std::random_device rd;
    default_random_engine re(rd());
    uniform_real_distribution<double> unif(0, 10*n);
    for (int i = 0; i<n; i++){
        x[i] = unif(re);
    }

    ArraySort<double> Y(x,n);
    X = Y;
    }


    X.quicksort_mpi();
    X.print();

    Finalize_parallel();
    return 0;
}