//Merge Sort Parallel Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

const int RMAX = 100;

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min);
void Generate_list(int scrambledArray[], int n, int rank);
int  Compare(const void* a_p, const void* b_p);
void Print_list(int scrambledArray[], int n);
int  getN(int rank, MPI_Comm comm);
void mergeSort(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm);
void merge(int scrambledArray[], int temp1[], int temp2[], int size);
void Print_global_list(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm);

int main(int argc, char** argv){
    //Usual MPI init
    MPI_Init(&argc, &argv);
    int rank, numranks;
    int* scrambledArray;
    MPI_Comm comm;
    comm = MPI_COMM_WORLD;
  
    MPI_Comm_size(comm, &numranks);
    MPI_Comm_rank(comm, &rank);
    MPI_Status stat;

    int n = getN(rank, comm);
    int max = 100; 
    int min = 0;

    scrambledArray = malloc(numranks*n*sizeof(int));
    Generate_list(scrambledArray, n, rank);

    //generateRandomNumbers(scrambledArray, n, max, min);

    Print_global_list(scrambledArray, n, rank, numranks, comm);

    mergeSort(scrambledArray, n, rank, numranks, comm);

    if (rank == 0) {
        Print_list(scrambledArray, numranks*n);
    }

    free(scrambledArray);

    MPI_Finalize();

    return 0;
} 

int getN(int rank, MPI_Comm comm) {
    int n;
    if (rank == 0) {
        n = 10;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, comm);
    return n;
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    int i;
    for (i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}

void Generate_list(int scrambledArray[], int n, int rank) {
  int i;
   /*for(i = 0; i < n; i++){
       scrambledArray[i]=i+2;
   } */
   srandom(rank+1);
   for (i = 0; i < n; i++)
      scrambledArray[i] = rand() % (RMAX + 1);
   qsort(scrambledArray, n, sizeof(int), Compare); 
}

int Compare(const void* a_p, const void* b_p) {
   int a = *((int*)a_p);
   int b = *((int*)b_p);

   if (a < b)
      return -1;
   else if (a == b)
      return 0;
   else 
      return 1;
} 

void Print_global_list(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm) {
   int* temp = NULL;

   if (rank == 0) {
      temp = malloc(numranks*n*sizeof(int));
      MPI_Gather(scrambledArray, n, MPI_INT, temp, n, MPI_INT, 0, comm);
      Print_list(temp, numranks*n);
      free(temp);
   } else {
      MPI_Gather(scrambledArray, n, MPI_INT, temp, n, MPI_INT, 0, comm);
   }
}

void Print_list(int scrambledArray[], int n) {
   int i;
   for (i = 0; i < n; i++)
      printf("%d ", scrambledArray[i]);
   printf("\n");
}

void mergeSort(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm) {
   int partner, done = 0, size = n;
   unsigned bitmask = 1;
   int *temp1, *temp2;
   MPI_Status status;

   temp1 = malloc(numranks*n*sizeof(int));
   temp2 = malloc(numranks*n*sizeof(int));
    
   while (!done && bitmask < numranks) {
      partner = rank ^ bitmask;
      if (rank > partner) {
         MPI_Send(scrambledArray, size, MPI_INT, partner, 0, comm);
         done = 1;
      } else {
         MPI_Recv(temp1, size, MPI_INT, partner, 0, comm, &status);
         merge(scrambledArray, temp1, temp2, size);
         size = 2*size;
         bitmask <<= 1;
      }
   }

   free(temp1);
   free(temp2);
}
void merge(int scrambledArray[], int temp1[], int temp2[], int size) {
   int ai, bi, ci;

   ai = bi = ci = 0;
   while (ai < size && bi < size)
     if (scrambledArray[ai] <= temp1[bi]) {
         temp2[ci] = scrambledArray[ai];
         ci++; ai++;
      } else {
         temp2[ci] = temp1[bi];
         ci++; bi++;
      }

   if (ai >= size)
      for (; ci < 2*size; ci++, bi++)
         temp2[ci] = temp1[bi];
   else
      for (; ci < 2*size; ci++, ai++)
         temp2[ci] = scrambledArray[ai];

   memcpy(scrambledArray, temp2, 2*size*sizeof(int));
}

