#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Counting Sort Serial Program

int *countingSort(int* array, int n);

int main(int argc, char** argv){

    int n = 10; //n is the number of elements in the array
    int scrambledArray[] = {6,2,7,4,2,12,9,1,4,3};

    //Prints out the scrambled array
    printf("The scrambled array is: \n");
    for (int i = 0; i  < n; i = i + 1) {
        printf("%d, ", scrambledArray[i]);
    }
    printf("\n");

    int *sortedArray = countingSort(scrambledArray, n);

    //Prints out the sorted array
    printf("The sorted array is: \n");
    for (int i = 0; i  < n; i = i + 1) {
        printf("%d, ", sortedArray[i]);
    }
    printf("\n");
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
