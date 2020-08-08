//quicksort Serial 
//Resources: geeksforgeeks.org
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void swap(int* a, int* b);
int partition (int arr[], int low, int high);
void quickSort(int arr[], int low, int high);
int *generateRandomNumbers(int *scrambledArray, int n, int max, int min);

int main(int argc, char** argv){

    //int n = 1000000000; //1billion
    int n = 100;
    int max = 100;
    int min = 0;
    srand(6);

    int *scrambledArray = malloc(n*sizeof(int));
    generateRandomNumbers(scrambledArray, n, max, min);

    //print the scrambled array
    printf("The scrambled array is:\n");
    for (int i = 0; i < n; i = i + 1) {
        printf("%d ", scrambledArray[i]);
    }
    printf("\n");

    quickSort(scrambledArray, 0, n-1);

    //print the sorted array despite it still being named scrambledArray
    printf("The sorted array is:\n");
    for (int i = 0; i < n; i = i + 1) {
        printf("%d ", scrambledArray[i]);
    }
    printf("\n");
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