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

    if (numranks%2==1) {
        return 0;
    }
    if (numranks == 6) {
        return 0;
    }
    if (numranks == 10) {
        return 0;
    }
    if (numranks == 12) {
        return 0;
    }

    double startTime = MPI_Wtime();
    int n;
    int pivot;
    int test = 0;
    //int *testArray = malloc((n)*sizeof(int));
    /***********************************************************************
     * I made this little section because these are the only variables we
     * should be changing to test, time, or play around with
    */
    if (rank ==0) {
        //n is the number of elements in the array
        //n = 1000000000;  //1billion
        //n = 100000000; //100million
        n = 2000000; //1 million
        //n = 12000; 
    }
    int max = 100; //the highest value you could possibly have to sort
    int min = 1; //please dont change this to 0 for right now
    //srand ( time ( NULL)); //different random numbers every run
    srand(5); //un-comment this if we want same set of numbers every run (good for timings?)
    int printOutStuff = 0; //0 is false, 1 is true
    /***********************************************************************/

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int remainder = ((n)%numranks);
    int total = 0;
    int *scrambledArray = malloc(n*sizeof(int)); //the original starting array with the random ints
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

    
    //the next stesp we want each rank do quicksort on their piece of an array
    if (rank == 0) { //choose a pivot from a rank and bcast to everyone
        //int pivot = myN-1;
        pivot = 50;
    }
    MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
    

    int *myLowList = malloc(n*sizeof(int));
    int *myHighList = malloc(n*sizeof(int));
    int *myLowList1 = malloc(n*sizeof(int)); //1-25
    int *myLowList2 = malloc(n*sizeof(int)); //25-50
    int *myHighList1 = malloc(n*sizeof(int)); //50-75
    int *myHighList2 = malloc(n*sizeof(int)); //75-101

    int lowListCounter = 0;
    int highListCounter = 0;
    int lowList1Counter = 0;
    int lowList2Counter = 0;
    int highList1Counter = 0;
    int highList2Counter = 0;

    int counter = 0;
    int halfNumRanks = numranks/2;
    int *newScrambledList = malloc(n*sizeof(int));
    //each rank divides unsorted list into low/lower than pivot and high/higher than pivot
    if (numranks == 2) {
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
            printf("Rank %d: myHighList is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myHighList[i]);
                
            }
            printf("\n");

            printf("Rank %d: myLowList is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myLowList[i]);
            }
            printf("\n");
        }
        
        
        int numValues = 0;
        if (rank >= halfNumRanks) { //if rank is higher end 
            
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

            if (numranks == 4) { //split the 2 groups into 4 groups now

            }
        }
        else if (rank < halfNumRanks) { //if rank is on the lower end 0,1
            
            for (int i = 0; i < lowListCounter; i = i + 1) {
                newScrambledList[i] = myLowList[i];
                counter = counter + 1;
            } 
            MPI_Recv(&lowListCounter, 1, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList, lowListCounter, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD, NULL);

            MPI_Send(&highListCounter, 1, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList, highListCounter, MPI_INT, rank+halfNumRanks, 0, MPI_COMM_WORLD);      

            if (printOutStuff == 1) {
                printf("Rank %d: I received lowListCounter from other rank and it is: %d\n", rank, lowListCounter);
                printf("Rank %d: I received myLowList from other rank and it contains: ", rank);
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
        
        if (printOutStuff == 1) {
            printf("Rank %d: newScrambledList before sorting is: ", rank);
            for (int i = 0; i < counter; i = i + 1) {
                printf("%d ", newScrambledList[i]);
            }
            printf("\n");
        }
        //At this point, each rank should sort their new values
        
        quickSort(newScrambledList, 0, counter-1);

        if (printOutStuff == 1) {
            printf("Rank %d: counter = %d\n", rank, counter);
            printf("Rank %d: my newScrambledList after sorting is: ", rank);
            for (int i = 0; i < counter; i = i + 1 ) {
                printf("%d ",newScrambledList[i]);
            }
            printf("\n");
        }
    } //end if numranks == 2

    if (numranks == 4) {
        
        for (int i = 0; i < myN; i = i + 1) {
            if (myScrambledArray[i] <= 25) {
                myLowList1[lowList1Counter] = myScrambledArray[i];
                lowList1Counter = lowList1Counter + 1;
            }
            else if (myScrambledArray[i] <= 50) {
                myLowList2[lowList2Counter] = myScrambledArray[i];
                lowList2Counter = lowList2Counter + 1;
            }
            else if (myScrambledArray[i] <= 75) {
                myHighList1[highList1Counter] = myScrambledArray[i];
                highList1Counter = highList1Counter + 1;
            }
            else if (myScrambledArray[i] <= 102) {
                myHighList2[highList2Counter] = myScrambledArray[i];
                highList2Counter = highList2Counter + 1;
            }
        }
        
        if (printOutStuff == 1) {
            //printf("Rank %d: pivot = %d\n", rank, pivot);
            printf("Rank %d: myHighList1 is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myHighList1[i]);
                
            }
            printf("\n");

            printf("Rank %d: myHighList2 is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myHighList2[i]);
                
            }
            printf("\n");
            
            printf("Rank %d: myLowList1 is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myLowList1[i]);
            }
            printf("\n");

            printf("Rank %d: myLowList2 is: ", rank);
            for (int i = 0; i < myN; i = i + 1) {
                printf("%d ", myLowList2[i]);
            }
            printf("\n");
        }
        int halfNumRanks = numranks/2;
        int *newScrambledList = malloc(n*sizeof(int));
        int counter = 0;
        int numValues = 0;
        if (rank == 0) { //if rank is 0
            printf("does this print---------------------\n");
            //fill array with own values first
            for (int i = 0; i < lowList1Counter; i = i + 1) {
                newScrambledList[i] = myLowList1[i];
                counter = counter + 1;
            }
            
            MPI_Send(&lowList2Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList2, lowList2Counter, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(&highList1Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList1, highList1Counter, MPI_INT, 2, 0, MPI_COMM_WORLD);
            MPI_Send(&highList2Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList2, highList2Counter, MPI_INT, 3, 0, MPI_COMM_WORLD);
            
            MPI_Recv(&lowList1Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList1, lowList1Counter, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList to newScrambledList
            for (int i = 0; i < lowList1Counter; i = i + 1) {
                newScrambledList[counter] = myLowList1[i];
                counter = counter + 1;
            }
            MPI_Recv(&lowList1Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList1, lowList1Counter, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList to newScrambledList
            for (int i = 0; i < lowList1Counter; i = i + 1) {
                newScrambledList[counter] = myLowList1[i];
                counter = counter + 1;
            }
            MPI_Recv(&lowList1Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList1, lowList1Counter, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);
            

            //concatenate the newly received myLowList1 to newScrambledList
            for (int i = 0; i < lowList1Counter; i = i + 1) {
                newScrambledList[counter] = myLowList1[i];
                counter = counter + 1;
            }

        }
        if (rank == 1) {
            //fill array with own values first
            for (int i = 0; i < lowList2Counter; i = i + 1) {
                newScrambledList[i] = myLowList2[i];
                counter = counter + 1;
            }
            
            MPI_Recv(&lowList2Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList2, lowList2Counter, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < lowList2Counter; i = i + 1) {
                newScrambledList[counter] = myLowList2[i];
                counter = counter + 1;
            }

            MPI_Send(&lowList1Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList1, lowList1Counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&highList1Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList1, highList1Counter, MPI_INT, 2, 0, MPI_COMM_WORLD);
            MPI_Send(&highList2Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList2, highList2Counter, MPI_INT, 3, 0, MPI_COMM_WORLD);

            MPI_Recv(&lowList2Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList2, lowList2Counter, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < lowList2Counter; i = i + 1) {
                newScrambledList[counter] = myLowList2[i];
                counter = counter + 1;
            }

            MPI_Recv(&lowList2Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myLowList2, lowList2Counter, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < lowList2Counter; i = i + 1) {
                newScrambledList[counter] = myLowList2[i];
                counter = counter + 1;
            }
        }
        if (rank == 2) {
            //fill array with own values first
            for (int i = 0; i < highList1Counter; i = i + 1) {
                newScrambledList[i] = myHighList1[i];
                counter = counter + 1;
            }

            MPI_Recv(&highList1Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList1, highList1Counter, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList1Counter; i = i + 1) {
                newScrambledList[counter] = myHighList1[i];
                counter = counter + 1;
            }

            MPI_Recv(&highList1Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList1, highList1Counter, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList1Counter; i = i + 1) {
                newScrambledList[counter] = myHighList1[i];
                counter = counter + 1;
            }

            MPI_Send(&lowList1Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList1, lowList1Counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&lowList2Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList2, lowList2Counter, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(&highList2Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList2, highList2Counter, MPI_INT, 3, 0, MPI_COMM_WORLD);

            MPI_Recv(&highList1Counter, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList1, highList1Counter, MPI_INT, 3, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList1Counter; i = i + 1) {
                newScrambledList[counter] = myHighList1[i];
                counter = counter + 1;
            }

        }

        if (rank == 3) {
            //fill array with own values first
            for (int i = 0; i < highList2Counter; i = i + 1) {
                newScrambledList[i] = myHighList2[i];
                counter = counter + 1;
            }

            MPI_Recv(&highList2Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList2, highList2Counter, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList2Counter; i = i + 1) {
                newScrambledList[counter] = myHighList2[i];
                counter = counter + 1;
            }

            MPI_Recv(&highList2Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList2, highList2Counter, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList2Counter; i = i + 1) {
                newScrambledList[counter] = myHighList2[i];
                counter = counter + 1;
            }

            MPI_Recv(&highList2Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(myHighList2, highList2Counter, MPI_INT, 2, 0, MPI_COMM_WORLD, NULL);

            //concatenate the newly received myLowList2 to newScrambledList
            for (int i = 0; i < highList2Counter; i = i + 1) {
                newScrambledList[counter] = myHighList2[i];
                counter = counter + 1;
            }

            MPI_Send(&lowList1Counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList1, lowList1Counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&lowList2Counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(myLowList2, lowList2Counter, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(&highList1Counter, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            MPI_Send(myHighList1, highList1Counter, MPI_INT, 2, 0, MPI_COMM_WORLD);
        }
        
        if (printOutStuff == 1) {
            printf("Rank %d: newScrambledList before sorting is: ", rank);
            for (int i = 0; i < counter; i = i + 1) {
                printf("%d ", newScrambledList[i]);
            }
            printf("\n");
        }
        //At this point, each rank should sort their new values
        
        quickSort(newScrambledList, 0, counter-1);

        if (printOutStuff == 1) {
            printf("Rank %d: counter = %d\n", rank, counter);
            printf("Rank %d: my newScrambledList after sorting is: ", rank);
            for (int i = 0; i < counter; i = i + 1 ) {
                printf("%d ",newScrambledList[i]);
            }
            printf("\n");

        }
        if (rank ==0) {
            if (printOutStuff == 1) {
                printf("rank zeroooo: counter = %d\n", counter);
            }
            test = counter;

            // for (int i = 0; i < counter; i = i + 1) {
            //     testArray[i] = newScrambledList[i];
            // }
        }
        
        if (rank != 0) { //if rank is higher end //if rank isnt zero
            printf("Rank %d: my counter= %d here \n", rank, counter);
            MPI_Send(&counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(newScrambledList, counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
            
        }
        
    } //end if numranks == 4

    //send the sorted scrambledList to rank 0
    if (numranks == 2) {
        if (rank != 0) { //if rank is higher end //if rank isnt zero
            //printf("Rank %d: my counter= %d here \n", rank, counter);
            MPI_Send(&counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(newScrambledList, counter, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    //have rank 0 concatenate the arrays from each rank together
    
    if (rank == 0) { //rank zero will do most of the finishing work at the end here
        int *unsortedArray = malloc(n*sizeof(int));
        int lastCounter = 0;
        int *sortedArray = malloc(n*sizeof(int));
  
        if (numranks == 4) {
            
            counter = test;
            //fill unsortedArray with rank 0's values first
            for (int i = 0; i < test; i = i + 1) {
                unsortedArray[i] = myScrambledArray[i];
                lastCounter = lastCounter + 1;
            }
            
        }
        if (numranks == 2) {
            for (int i = 0; i < counter; i = i + 1) {
                unsortedArray[i] = newScrambledList[i];
                lastCounter = lastCounter + 1;
            }
        }
        if (printOutStuff == 1) {
            for (int i = 0; i < counter; i = i + 1) {
                printf("%d ", unsortedArray[i]);
            }
            printf("\n");
        }
        
        //rank 0 receives the sorted array piece from the all ranks
        int rankCounter = 1;
        
        while (rankCounter < numranks) { //issue in here
            MPI_Recv(&counter, 1, MPI_INT, rankCounter, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(newScrambledList, counter, MPI_INT, rankCounter, 0, MPI_COMM_WORLD, NULL);
            
            if (printOutStuff == 1) {
                printf("Rank %d: I received newScrambledList from a rank and it contains: ", rank);
                for (int i = 0; i < counter; i = i + 1) {
                    printf("%d ", newScrambledList[i]);
                }
                printf("\n");
            }
            
            int x = 0;
            int loopEndCondition = lastCounter + counter;
            for (int i = lastCounter; i < loopEndCondition; i = i + 1) {
                unsortedArray[i] = newScrambledList[x];
                lastCounter = lastCounter + 1;
                x = x + 1;
            }
            rankCounter = rankCounter + 1;
            
        }
        
        //print the almost sorted array
        if (printOutStuff == 1) {
            printf("The almost sorted array is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", unsortedArray[i]);
            }
            printf("\n");
        }
        
        //print the sorted array
        if (printOutStuff == 1) {
            printf("The sorted array is:\n");
            for (int i = 0; i < n; i = i + 1) {
                printf("%d ", unsortedArray[i]); //ignore the naming here
                 if (i%30 == 29) {
                     printf("\n");
                 }
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
} //end program
