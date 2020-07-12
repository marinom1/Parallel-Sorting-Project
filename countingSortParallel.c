//Counting Sort Parallel Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
//Counting Sort Serial Program
int* generateRandomNumbers(int* scrambledArray, int n, int max, int min);
int *countingSort(int* array, int n);

int main(int argc, char** argv){

    //Usual MPI init
    MPI_Init(&argc, &argv);
    int rank, numranks;
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    int n;
    int total;
    if (rank == 0) {
        n = 10; //n is the number of elements in the array
    }
    int remainder = ((n)%numranks);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *sendcounts = malloc(n*sizeof(int)); 
	int *displs = malloc(n*sizeof(int));

    for (int i = 0; i < numranks; i = i + 1) {
        sendcounts[i] = n / numranks;
        if (remainder > 0) {
            sendcounts[i] = sendcounts[i] + 1;
            remainder = remainder - 1;
        }

        displs[i] = total;
        total = total + sendcounts[i];
    }

    if (rank == 0) {
        for (int i = 0; i < numranks; i = i + 1) {
            printf("sendcounts[%d] = %d    -   displs[%d] = %d\n", i, sendcounts[i], i, displs[i]);
        }
    }

    MPI_Bcast(sendcounts, numranks, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs, numranks, MPI_INT, 0, MPI_COMM_WORLD);

    int myN =sendcounts[rank];

    //int scrambledArray[] = {6,2,7,4,2,12,9,1,4,3}; //small test sample
    
    int max = 100;
    int min = 0;

   // srand ( time ( NULL)+rank); //this makes a different seed for the randomn number generator everytime. maybe not good if we want to limit variables for timings
    srand(5); //un-comment this if we want same set of numbers every run (good for timings?)
    int *scrambledArray = malloc(n*sizeof(int));
    if (rank ==0) {
        generateRandomNumbers(scrambledArray, n, max, min);
    }
    
    //Prints out the scrambled array
    if (rank == 0) {
        printf("The scrambled array is: \n");
        for (int i = 0; i  < n; i = i + 1) {
            printf("%d ", scrambledArray[i]);
                if (i % 30 == 29 ) {
                    printf("\n");
            }
        }
    }
    
    printf("\n"); 

    MPI_Bcast(scrambledArray, n, MPI_INT, 0, MPI_COMM_WORLD);

    int count = 0;
    int *myScrambledArray = malloc(myN*sizeof(int));

    //this gives each rank their own piece of the scambled array
    if (rank == numranks-1) {
        for (int i = displs[rank]; i < n; i = i + 1) {
            myScrambledArray[count] = scrambledArray[i];
            count = count + 1;
        }
    }
    else {
        for (int i = displs[rank]; i < displs[rank+1]; i = i + 1) {
            myScrambledArray[count] = scrambledArray[i];
            count = count + 1;
        }
    }

    //Prints out the myScrambled array
    printf("Rank %d: myScrambledArray contains: \n", rank);
    for (int i = 0; i  < myN; i = i + 1) {
        printf("%d ", myScrambledArray[i]);
        if (i % 30 == 29 ) {
            printf("\n");
        }
    }
    printf("\n");
    
    int *mySortedArray = countingSort(myScrambledArray, myN);

    //Prints out the sorted array
   /* printf("The sorted array is: \n");
    for (int i = 0; i  < n; i = i + 1) {
        printf("%d ", sortedArray[i]);
        if (i % 30 == 29 ) {
            printf("\n");
        }
    }
    printf("\n"); */
    MPI_Finalize();
}

int * countingSort(int *array, int n) {
    int *sortedArray = malloc(n*sizeof(int));
    //First gotta find largest number in the array
    int max = array[0];
    for (int i = 1; i < n; i = i + 1) {
        if (array[i] > max) {
             max = array[i];
        }
    }
    //Initialize countArray and make every entry 0 to start off
    int countArray[max+1];
    for (int i = 0; i <= max; ++i) {
        countArray[i] = 0;
    }

    //Next, store how many times each int appears
    for (int i = 0; i < n; i = i + 1) {
        countArray[array[i]] = countArray[array[i]] + 1;
    }

    //Then, store the cumulative count of each array
    for (int i = 1; i <= max; i = i + 1) {
        countArray[i] += countArray[i - 1];
    }

    //Then, find index of each element from original array into count array, then puts the elements in sortedArray
    for (int i = n - 1; i >= 0; i = i - 1) {
        sortedArray[countArray[array[i]] - 1] = array[i];
        countArray[array[i]] = countArray[array[i]] - 1;
    }
    
    return (int*) sortedArray;
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    
    for (int i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}