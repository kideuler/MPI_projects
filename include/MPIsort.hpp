#ifndef MPI_SORT
#define MPI_SORT
#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>


#ifdef USE_MPI
#include <mpi.h>
#endif

using namespace std;

void Init_parallel();
void Finalize_parallel();
int get_rank();

template <typename T>
class ArraySort {
    private:
        T* arr;
        int size;

        int quicksort_partition(int lo, int hi){
            int pivot = arr[hi];

            int i = lo - 1;
            T temp;
            for (int j = lo; j<hi; j++){
                if (arr[j] <= pivot){
                    i = i+1;
                    temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
            }

            i=i+1;
            temp = arr[i];
            arr[i] = arr[hi];
            arr[hi] = temp;
            return i;
        }

        void quicksort_kernel(int lo, int hi){
            if (lo >= hi || lo < 0){
                return;
            }

            int p = quicksort_partition(lo,hi);
            quicksort_kernel(lo,p-1);
            quicksort_kernel(p+1,hi);
            return;
        }

        void merge(const ArraySort<T> &X){
            int i=0,j=0,k=0;
            int newsize = size + X.size;
            T* newarr = new T[newsize];
            while (k < size+X.size){
                if (i >=size){
                    newarr[k] = X.arr[j];
                    j++;
                } else if (j >= X.size){
                    newarr[k] = arr[i];
                    i++;
                } else {
                    if (arr[i] <= X.arr[j]){
                        newarr[k] = arr[i];
                        i++;
                    } else {
                        newarr[k] = X.arr[j];
                        j++;
                    }
                }
                k++;
            }
            delete [] arr;
            arr = newarr;
            size = newsize;
            return;
        }

    public:
        // constructor
        ArraySort(){
            arr = new T;
            size = 0;
        
        }
        ArraySort(T x[], int sz){
            arr = new T[sz];
            size = sz;
            for (int i = 0; i<size; i++){
                arr[i] = x[i];
            }
        }

        ArraySort<T> &operator=(const ArraySort<T> &rhs){
            size = rhs.size;
            arr = new T[size];
            for (int i = 0; i<size; i++){
                arr[i] = rhs.arr[i];
            }
            return *this;
        }

        // destructor
        ~ArraySort(){ delete[] arr; size = 0;}

        // member functions
        void print(){
            for (int i = 0; i<size; i++){
                cout << arr[i] << endl;
            }
        }

        void quicksort(){
            quicksort_kernel(0,size-1);
        }

        void quicksort_mpi(){
        #ifdef USE_MPI
            int size_part, nrank, size_1;
            T* xbuff;
            int rank = get_rank();
            MPI_Comm_size(MPI_COMM_WORLD, &nrank);
            double log2n = log2(double(nrank));
            int niters = int(log2n);
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank == 0 ){
                size_part = floor(size / nrank);
                size_1 = size - size_part*(nrank-1);

                xbuff = new T[size_1];
                for (int i = 0; i<size_1; i++){
                    xbuff[i] = arr[i];
                }
                
                int ii = size_1;
                T* xbuff4 = new T[size_part];
                    for (int r = 1; r<nrank; r++){
                        for (int i = 0; i<size_part; i++){
                            xbuff4[i] = arr[ii+(r-1)*size_part + i];
                        }
                        MPI_Send(xbuff4,size_part,MPI_DOUBLE_PRECISION,r,0,MPI_COMM_WORLD);
                    }
                delete [] xbuff4;
            }
            MPI_Bcast(&size_part,1,MPI_INT,0,MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Status status;
            // recieve data from master
            if (rank != 0){
                xbuff = new T[size_part];
                MPI_Recv(xbuff, size_part, MPI_DOUBLE_PRECISION, 0, 0, MPI_COMM_WORLD, &status);
            }
            ArraySort<T> Arrbuff(xbuff, rank==0 ? size_1 : size_part);
            delete[] xbuff;

            Arrbuff.quicksort();

            MPI_Barrier(MPI_COMM_WORLD);
            int offset, nv;
            T* xbuff2;
            ArraySort<T> Arrbuff2;
            for (int iter = 0; iter<niters; iter++){
                offset = pow(2,iter);
                nv = size_part*offset;
                for(int r = 0; r<nrank; r+=pow(2,iter+1)){

                    // send high to low
                    if (rank == r+offset){
                        MPI_Send(Arrbuff.arr,nv,MPI_DOUBLE_PRECISION,r,0,MPI_COMM_WORLD);
                    }

                    if (rank == r){
                        xbuff2 = new T[nv];
                        MPI_Recv(xbuff2,nv,MPI_DOUBLE_PRECISION,r+offset,0,MPI_COMM_WORLD,&status);
                        Arrbuff2 = ArraySort<T>(xbuff2,nv); 
                        Arrbuff.merge(Arrbuff2);
                        delete [] xbuff2;
                    }
                }
            }

            
            delete [] arr;
            arr = Arrbuff.arr;
            size = Arrbuff.size; 
        #else
            quicksort();
        #endif
        }

        
};

#endif