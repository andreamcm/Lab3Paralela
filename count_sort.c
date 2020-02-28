#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);
void Count_sort(int a[], int n, int thread_count);

int main(int argc, char* argv[]) {                 
   int a[10] = {7,2,1,4,9,3,5,6,10,0};
   int n, thread_count;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   Count_sort(a, n, thread_count);

   return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}  /* Usage */

/*--------------------------------------------------------------------
 * Function:    Count_Sort
 * Purpose:     Sort an array 
 * In arg:      a, n thread_count
 */
void Count_sort(int a[], int n, int thread_count){
    int i, j, count;
    int* temp = malloc(n*sizeof(int));

#   pragma omp parallel for num_threads(thread_count)
    for (i=0; i < n; i++){
        count = 0;
        for(j = 0; j < n; j++)
            if(a[j] < a[i])
                count++;
            else if (a[j] == a[i] && j < i)
                count++;
        temp[count] = a[i];
    }
    memcpy(a, temp, n*sizeof(int));
    free(temp);
}