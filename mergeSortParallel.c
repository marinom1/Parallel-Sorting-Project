//Merge Sort Parallel Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

const int MAX = 100;

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min);
int  Compare(const void* a_p, const void* b_p);
void Print_list(int scrambledArray[], int n);
int  getN(int rank, MPI_Comm comm);
void mergeSort(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm);
void merge(int scrambledArray[], int temp1[], int temp2[], int size);
void Print_global_list(int scrambledArray[], int n, int rank, int numranks, MPI_Comm comm);

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    double progStart = MPI_Wtime();
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
    int print;

    if(n > 100) {
       print = 0;
    } else {
       print = 1;
    }
    scrambledArray = malloc(numranks*n*sizeof(int));

    double starttime = MPI_Wtime();
    generateRandomNumbers(scrambledArray, n*numranks, max, min);
    double endtime = MPI_Wtime();
    
   double generateTime = endtime-starttime;

    if (rank == 0) {
        if(print == 1) {
          printf("Unsorted List\n");
          Print_list(scrambledArray, numranks*n);
        }
    }

    mergeSort(scrambledArray, n, rank, numranks, comm);

    if (print == 1) {
       Print_global_list(scrambledArray, n, rank, numranks, comm);
    }   



   if(rank == 0) {
        printf("\nProgram with n=%d ran successfully\n", n);
   }

    double progEnd = MPI_Wtime();
    printf("Rank: %d, Time: %.5f, gen: %.5f\n", rank, (progEnd-progStart),generateTime); 

    free(scrambledArray);

    MPI_Finalize();

    return 0;
} 

int getN(int rank, MPI_Comm comm) {
    int n;
    if (rank == 0) {
        n = 1000000;
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
      qsort(temp, n*numranks, sizeof(int), Compare);
      printf("Sorted List\n");
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
   int partner, done = 0;
   unsigned var = 1;
   int *temp1, *temp2;
   MPI_Status status;

   temp1 = malloc(numranks*n*sizeof(int));
   temp2 = malloc(numranks*n*sizeof(int));
    
   while (!done && var < numranks) {
      partner = rank ^ var;
      if (rank > partner) {
         MPI_Send(scrambledArray, n, MPI_INT, partner, 0, comm);
         done = 1;
      } else {
         MPI_Recv(temp1, n, MPI_INT, partner, 0, comm, &status);
         merge(scrambledArray, temp1, temp2, n);
         n = 2*n;
         var <<= 1;
      }
   }

   free(temp1);
   free(temp2);
}
void merge(int scrambledArray[], int temp1[], int temp2[], int n) {
   int a, b, c;

   a = b = c = 0;
   while (a < n && b < n)
     if (scrambledArray[a] <= temp1[b]) {
         temp2[c] = scrambledArray[a];
         c++; a++;
      } else {
         temp2[c] = temp1[b];
         c++; b++;
      }

   if (a >= n)
      for (; c < 2*n; c++, b++)
         temp2[c] = temp1[b];
   else
      for (; c < 2*n; c++, a++)
         temp2[c] = scrambledArray[a];

   memcpy(scrambledArray, temp2, 2*n*sizeof(int));
}

