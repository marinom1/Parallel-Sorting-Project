//Merge Sort Serial Program
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
 
int *generateRandomNumbers(int *scrambledArray, int n, int max, int min);

void mergeSort(int i, int j, int scrambledArray[], int tempArray[]) {

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

int main() {
    int n = 100;
    int max = 100;
    int min = 0;
    int i;

    int *scrambledArray = malloc(n*sizeof(int));
    int *tempArray = malloc(n*sizeof(int));
    generateRandomNumbers(scrambledArray, n, max, min);
    
    printf("The scrambled array is:\n");
    for (i = 0; i < n; i++) {
        printf("%d ", scrambledArray[i]);
    }
    
    mergeSort(0, n - 1, scrambledArray, tempArray);
    
    printf("\nThe sorted array is:\n"); 
    for (i = 0; i < n; i++) {
        printf("%d ", scrambledArray[i]);
    }
    
    return 0;
}

int* generateRandomNumbers(int *scrambledArray, int n, int max, int min) {
    for (int i = 0; i < n; i = i + 1) {
        scrambledArray[i] = rand() % (max + 1) + min;
    }
}
