//Counting Sort Parallel Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int *generateRandomNumbers(int *scrambledArray, int n, int max, int min);
int *countingSort(int *array, int n, int biggestValue, int max, int* countArray);


int main(int argc, char** argv){

    //Usual MPI init
    MPI_Init(&argc, &argv);
    int rank, numranks;
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    int n;

    /***********************************************************************
     * I made this little section because these are the only variables we
     * should be changing to test, time, or play around with
    */
    if (rank ==0) {
        //n is the number of elements in the array
        n = 1000000000;  //1billion
        //n = 100000000; //100million
        //n = 50;
    }
    int max = 100; //the highest value you could possibly have to sort
    int min = 0; //please dont change this, program as is can't handle other min values
    //srand ( time ( NULL)); //different random numbers every run
    srand(5); //un-comment this if we want same set of numbers every run (good for timings?)
    int printOutStuff = 0; //0 is false, 1 is true
    /***********************************************************************/

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int remainder = ((n)%numranks);
    int total = 0;
    int *scrambledArray = malloc(n*sizeof(int));
    int biggestValue;
    int *sendcounts = malloc(n*sizeof(int)); 
	int *displs = malloc(n*sizeof(int));
    
    generateRandomNumbers(scrambledArray, n, max, min);

    //Prints out the scrambled array
    if (printOutStuff == 1) {
        if (rank ==0) {
            printf("The scrambled array is: ");
            for (int i = 0; i  < n; i = i + 1) {
                printf("%d ", scrambledArray[i]);
                if (i % 30 == 29 ) {
                    printf("\n");
                }
            }
    printf("\n");
        } 
    }
     
    for (int i = 0; i < numranks; i = i + 1) { //calculate what piece of work each rank gets
        sendcounts[i] = n / numranks;
        if (remainder > 0) {
            sendcounts[i] = sendcounts[i] + 1;
            remainder = remainder - 1;
        }

        displs[i] = total;
        total = total + sendcounts[i];
    }

    if (printOutStuff == 1) {
        if (rank == 0) { //prints out the calculations for how much work each rank will do
            for (int i = 0; i < numranks; i = i + 1) {
                printf("sendcounts[%d] = %d    -   displs[%d] = %d\n", i, sendcounts[i], i, displs[i]);
            }
        }
    }
    MPI_Bcast(sendcounts, numranks, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs, numranks, MPI_INT, 0, MPI_COMM_WORLD);
    int myN =sendcounts[rank];

    for (int i = 1; i < n; i = i + 1) { //Find the biggest value, so we know when to stop counting
        if (scrambledArray[i] > biggestValue) {
             biggestValue = scrambledArray[i];
        }
    }

    int *myScrambledArray = malloc(n*sizeof(int));
    int count = 0;
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
    if (printOutStuff == 1) {
        printf("Rank %d: myScrambledArray contains: ", rank);
        for (int i = 0; i  < myN; i = i + 1) {
            printf("%d ", myScrambledArray[i]);
            if (i % 30 == 29 ) {
                printf("\n");
            }
        }
    }
    printf("\n"); 

    //Parallel part here
    int *countArray = malloc((max+1)*sizeof(int));
    int *myCountArray = countingSort(myScrambledArray, myN, biggestValue, max, countArray);

    if (printOutStuff == 1) {
        printf("Rank %d: myCountArray is: ", rank);
        for (int i = 0; i < max+1; i = i + 1) {
                printf("%d ",myCountArray[i]);
        }
        printf("\n");
    }

    for (int i = 0; i < max+1; i = i + 1) { //this adds all the counts from all the ranks
        int x = myCountArray[i];
        MPI_Reduce(&x, &myCountArray[i],1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);
    }
   //end of parallel part
    //at this point, myCountArray contains all the counts from all ranks summed up
    //only want rank 0 doing the rest of the program
    
    if (rank ==0) {
        if (printOutStuff == 1) {
            printf("the big myCountArray is: ");
            for (int i = 0; i < max+1; i = i + 1) {
                printf("%d ",myCountArray[i]);
            }
    
        printf("\n"); 
        }
        //Then, store the cumulative count of each array
        
        for (int i = 1; i <= max; i = i + 1) {
            myCountArray[i] += myCountArray[i - 1];
        }
        if (printOutStuff == 1) {
        printf("the cumulative countArray is: ");
            for (int i = 0; i < max+1; i = i + 1) {
                printf("%d ",myCountArray[i]);
            }
        printf("\n"); 
        }
        //Then, find index of each element from original array into count array, then puts the elements in sortedArray
        int *sortedArray = malloc(n*sizeof(int));
        for (int i = n - 1; i >= 0; i = i - 1) {
            sortedArray[myCountArray[scrambledArray[i]] - 1] = scrambledArray[i];
            myCountArray[scrambledArray[i]] = myCountArray[scrambledArray[i]] - 1;
        }
        //sortedArray is sorted at this point
        //Prints out the sorted array
        if (printOutStuff == 1) {
        printf("The sorted array is: \n");
        for (int i = 0; i  < n; i = i + 1) {
            printf("%d ", sortedArray[i]);
            if (i % 30 == 29 ) {
                printf("\n");
            }
        }
        printf("\n");  
        }
    } //end if rank ==0

    if(rank ==0) {
        printf("Program with n=%d ran successfully", n);
    }
    
    MPI_Finalize();
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    for (int i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}

int *countingSort(int *array, int n, int biggestValue, int max, int* countArray){
    //Initialize countArray and make every entry 0 to start off
    for (int i = 0; i < max+1; i = i + 1) {
        countArray[i] = 0;
    }
    //Next, store how many times each int appears
    for (int i = 0; i < n; i = i + 1) {
        countArray[array[i]] = countArray[array[i]] + 1;
    }
    return (int*) countArray; 
} //end countingSort