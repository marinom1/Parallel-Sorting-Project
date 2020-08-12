//Quicksort Parallel Program
//Resources: geeksforgeeks.org
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
void swap(int* a, int* b);
int partition (int arr[], int low, int high);
void quickSort(int arr[], int low, int high);
int *generateRandomNumbers(int *scrambledArray, int n, int max, int min);

int main(int argc, char** argv){

    //Usual MPI init
    MPI_Init(&argc, &argv);
    int rank, numranks;
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    double startTime = MPI_Wtime();
    int n;
    int pivot;
    /***********************************************************************
     * I made this little section because these are the only variables we
     * should be changing to test, time, or play around with
    */
    if (rank ==0) {
        //n is the number of elements in the array
        //n = 1000000000;  //1billion
        //n = 100000000; //100million
        //n = 1000000; //1 million
        n = 50; //68000 max?
    }
    int max = 100; //the highest value you could possibly have to sort
    int min = 1; //please dont change this to 0 for right now
    //srand ( time ( NULL)); //different random numbers every run
    srand(5); //un-comment this if we want same set of numbers every run (good for timings?)
    int printOutStuff = 1; //0 is false, 1 is true
    /***********************************************************************/

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int remainder = ((n)%numranks);
    int total = 0;
    int *scrambledArray = malloc(n*sizeof(int));
    int biggestValue;
    int *sendcounts = malloc(n*sizeof(int)); 
	int *displs = malloc(n*sizeof(int));
    generateRandomNumbers(scrambledArray, n, max, min);
    
    //print the scrambled array
    if (rank == 0) {
        if (printOutStuff == 1) {
            printf("The scrambled array is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", scrambledArray[i]);
            }
            printf("\n");
        }
    }
    
    for (int i = 0; i < numranks; i = i + 1) { //calculate what piece of work each rank gets
        sendcounts[i] = (n / numranks);
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
        printf("Rank %d: myScrambledArray contains: \n", rank);
        for (int i = 0; i  < myN; i = i + 1) {
            printf("%d ", myScrambledArray[i]);
            if (i % 50 == 49 ) {
                printf("\n");
            }
        }
    }
    printf("\n");

    
    //Have each rank do quicksort on their piece of the array
    if (rank == 0) { //choose a pivot from a rank and bcast to everyone
        //int pivot = myN-1;
        pivot = 50;
    }
    MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
    

    int *myLowList = malloc(n*sizeof(int));
    int *myHighList = malloc(n*sizeof(int));

    int lowListCounter = 0;
    int highListCounter = 0;

    //each rank divides unsorted list into low/lower than pivot and high/higher than pivot
    for (int i = 0; i < myN; i = i + 1) {
        if (myScrambledArray[i] < pivot) {
            myLowList[lowListCounter] = myScrambledArray[i];
            lowListCounter = lowListCounter + 1;
        }
        else {
            myHighList[highListCounter] = myScrambledArray[i];
            highListCounter = highListCounter + 1;
        }
    }
    if (printOutStuff == 1) {
        printf("Rank %d: pivot = %d\n", rank, pivot);
        printf("Rank %d: myHighList is: \n", rank);
        for (int i = 0; i < myN; i = i + 1) {
            printf("%d ", myHighList[i]);
            
        }
        printf("\n");

        printf("Rank %d: myLowList is: \n", rank);
        for (int i = 0; i < myN; i = i + 1) {
            printf("%d ", myLowList[i]);
        }
        printf("\n");
    }
    int halfNumRanks = numranks/2;
    int *newScrambledList = malloc(n*sizeof(int));
    int counter = 0;
    int numValues = 0;
    if (rank >= halfNumRanks) { //if rank is higher end //nproblem good to here
        
        for (int i = 0; i < highListCounter; i = i + 1) {
            newScrambledList[i] = myHighList[i];
            counter = counter + 1;
        }
        
        MPI_Send(&lowListCounter, 1, MPI_INT, rank-halfNumRanks, 0, MPI_COMM_WORLD);
        MPI_Send(myLowList, lowListCounter, MPI_INT, rank-halfNumRanks, 0, MPI_COMM_WORLD);
        MPI_Recv(&highListCounter, 1, MPI_INT, rank-halfNumRanks, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(myHighList, highListCounter, MPI_INT, rank-halfNumRanks, 0, MPI_COMM_WORLD, NULL);

        //concatenate the newly received myHighList to newScrambledList
        for (int i = 0; i < highListCounter; i = i + 1) {
            newScrambledList[counter] = myHighList[i];
            counter = counter + 1;
        }
    }

    else if (rank < halfNumRanks) { //if rank is on the lower end
        
        for (int i = 0; i < lowListCounter; i = i + 1) {
            newScrambledList[i] = myLowList[i];
            counter = counter + 1;
        }    
        
        MPI_Send(&highListCounter, 1, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD);
        MPI_Send(myHighList, highListCounter, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD);    
        MPI_Recv(&lowListCounter, 1, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(myLowList, lowListCounter, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD, NULL);

        if (printOutStuff == 1) {
            printf("Rank %d: I received lowListCounter from other rank and it is: %d\n", rank, lowListCounter);
            printf("Rank %d: I received myLowList from other rank and it contains:\n", rank);
            for (int i = 0; i < lowListCounter; i = i + 1) {
                printf("%d ", myLowList[i]);
            }
            printf("\n");
        }
        //concatenate the newly received myLowList to newScrambledList
        for (int i = 0; i < lowListCounter; i = i + 1) {
            newScrambledList[counter] = myLowList[i];
            counter = counter + 1;
        }
    }
    //nproblem might be above here
    if (printOutStuff == 1) {
        printf("Rank %d: newScrambledList before sorting is: \n", rank);
        for (int i = 0; i < counter; i = i + 1) {
            printf("%d ", newScrambledList[i]);
        }
        printf("\n");
    }
    //At this point, each rank should sort their new values
    
    quickSort(newScrambledList, 0, counter-1);

    if (printOutStuff == 1) {
        printf("Rank %d: counter = %d\n", rank, counter);
        printf("Rank %d: my newScrambledList after sorting is: \n", rank);
        for (int i = 0; i < counter; i = i + 1 ) {
            printf("%d ",newScrambledList[i]);
        }
        printf("\n");
    }

    //send the sorted scrambledList to rank 0
    if (rank >= halfNumRanks) { //if rank is higher end
        MPI_Send(&counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(newScrambledList, counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    //MPI_Gather(newScrambledList, sendcounts[0], MPI_INT, recbuff, sendcounts[0], MPI_INT, 0, MPI_COMM_WORLD);

    /*if (rank == 0 ) {
        printf("recbuff[] is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", recbuff[i]);
            }
    } */
    
    //have rank 0 concatenate the arrays from each rank together
    int index = 0;
    if (rank == 0) {
        int *unsortedArray = malloc(n*sizeof(int));
        int lastCounter = 0;
        int sendRank;
        int *sortedArray = malloc(n*sizeof(int));

        //fill unsortedArray with rank 0 values
        for (int i = 0; i < counter; i = i + 1) {
            unsortedArray[i] = newScrambledList[i];
            lastCounter = lastCounter + 1;
        }

        //receive the sorted array piece from rank 1
        MPI_Recv(&counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(newScrambledList, counter, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
        
        if (printOutStuff == 1) {
            printf("Rank %d: I received newScrambledList from rank 1 and it contains: \n", rank);
            for (int i = 0; i < counter; i = i + 1) {
                printf("%d ", newScrambledList[i]);
            }
            printf("\n");

            printf("do we get to this point?____________________________\n");
        }
        int x = 0;
        for (int i = lastCounter; i < lastCounter+counter; i = i + 1) {
            unsortedArray[i] = newScrambledList[x];
            //lastCounter = lastCounter + 1;
            x = x + 1;
        }
        

    //print the recbuff array
        // if (printOutStuff == 1) {
        //     printf("The recbuff array is:\n");
        //     for (int i = 0; i < n; i = i + 1) {
        //         printf("%d ",recbuff[i]);
        //         if (i%30==29) {
        //             printf("\n");
        //         }
        //     }
        //     printf("\n");
        // }

        // for (int i = 0; i < n; i = i + 1) {
        //     sortedArray[i] = recbuff[i];
        // }


        //print the almost sorted array
        if (printOutStuff == 1) {
            printf("The almost sorted array is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", unsortedArray[i]);
            }
            printf("\n");
        }
        
        if (numranks > 2) {
            quickSort(unsortedArray, 0, n-1);
        }
        

        //print the sorted array
        if (printOutStuff == 1) {
            printf("The sorted array is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", unsortedArray[i]); //ignore the naming here
                
            }
            printf("\n");
        }

        double endTime = MPI_Wtime();
        printf("Program time: %f\n", endTime-startTime);
        printf("n = %d\n", n);
        printf("numranks = %d\n", numranks);

    }
    
    
    MPI_Finalize();
    return 0;
}

void swap(int* a, int* b) {
 
    int t = *a; 
    *a = *b; 
    *b = t; 
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high]; 
    int i = (low - 1);  // index of smaller element 
  
    for (int j = low; j <= high- 1; j++) {
    
        // if current element is smaller than pivot 
        if (arr[j] < pivot) 
        { 
            i = i + 1; 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        //pi is partitioning index and arr[p] is currently at right place 
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 

        quickSort(arr, pi + 1, high); 
    } 
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    for (int i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}
