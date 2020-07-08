#include <stdio.h>
#include <stdlib.h>
#include <math.h>



//Counting Sort Serial Program
int main(int argc, char** argv){

    int array[10] = {6,2,7,4,2,12,9,1,4,3};

    countingSort(array);

    printf("the scrambled array is: \n");
    for (int i = 0; i  < 10; i = i + 1) {
        printf("%d, ", array[i]);
    }
    printf("\n");
}
