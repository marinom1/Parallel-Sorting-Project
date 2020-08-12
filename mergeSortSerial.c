//Merge Sort Serial Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
 
int *generateRandomNumbers(int *scrambledArray, int n, int max, int min);

void *mergeSort(int i, int j, int scrambledArray[], int tempArray[]);

int main() {
    int n = 10000000;
    int max = 100;
    int min = 0;
    int i;
    int print;

    if(n > 10) {
       print = 0;
    } else {
       print = 1;
    }

    clock_t startTime, endTime;
    double totTime;

    int *scrambledArray = malloc(n*sizeof(int));
    int *tempArray = malloc(n*sizeof(int));
    generateRandomNumbers(scrambledArray, n, max, min);
    
    if(print == 1) {
        printf("The scrambled array is:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", scrambledArray[i]);
        }
    }
    
    startTime = clock();
    mergeSort(0, n - 1, scrambledArray, tempArray);
    endTime = clock();

    totTime = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    if(print == 1) {
        printf("\nThe sorted array is:\n"); 
        for (i = 0; i < n; i++) {
            printf("%d ", scrambledArray[i]);
        }
    }
    
    printf("\nTotal Time: %f\n", totTime);
    
    return 0;
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    int i;
    for (i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}

void *mergeSort(int i, int j, int scrambledArray[], int tempArray[]) {

    if (j <= i) {
        return;     
    }
    
    int mid = (i + j) / 2;
   
    mergeSort(i, mid, scrambledArray, tempArray);     
    mergeSort(mid + 1, j, scrambledArray, tempArray);     

    int l1 = i;       
    int l2 = mid + 1;        
    int k;      

    for (k = i; k <= j; k++) {
        if (l1 == mid + 1) {      
            tempArray[k] = scrambledArray[l2];
            l2++;
        } else if (l2 == j + 1) {       
            tempArray[k] = scrambledArray[l1];
            l1++;
        } else if (scrambledArray[l1] < scrambledArray[l2]) {        
            tempArray[k] = scrambledArray[l1];
            l1++;
        } else {       
            tempArray[k] = scrambledArray[l2];
            l2++;
        }
    }

    for (k = i; k <= j; k++) {     
        scrambledArray[k] = tempArray[k];
    }
}
