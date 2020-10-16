#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>


#define SIZE 50
#define LOWER -10000
#define UPPER 10000

int *firstHPart, *secondPart, *thirdPart, *fourthPart,* fifthPart;

int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* HEAP SORT */
// A heap has current size and array of elements

struct MaxHeap
{
    int size;
    int* array;
};

// A utility function to swap to integers
void swap(int* a, int* b) { int t = *a; *a = *b;  *b = t; }

// The main function to heapify a Max Heap. The function
// assumes that everything under given root (element at
// index idx) is already heapified
void maxHeapify(struct MaxHeap* maxHeap, int idx)
{
    int largest = idx;  // Initialize largest as root
    int left = (idx << 1) + 1;  // left = 2*idx + 1
    int right = (idx + 1) << 1; // right = 2*idx + 2

    // See if left child of root exists and is greater than
    // root
    if (left < maxHeap->size &&
        maxHeap->array[left] > maxHeap->array[largest])
        largest = left;

    // See if right child of root exists and is greater than
    // the largest so far
    if (right < maxHeap->size &&
        maxHeap->array[right] > maxHeap->array[largest])
        largest = right;

    // Change root, if needed
    if (largest != idx)
    {
        swap(&maxHeap->array[largest], &maxHeap->array[idx]);
        maxHeapify(maxHeap, largest);
    }
}

// A utility function to create a max heap of given capacity
struct MaxHeap* createAndBuildHeap(int *array, int size)
{
    int i;
    struct MaxHeap* maxHeap =
              (struct MaxHeap*) malloc(sizeof(struct MaxHeap));
    maxHeap->size = size;   // initialize size of heap
    maxHeap->array = array; // Assign address of first element of array

    // Start from bottommost and rightmost internal mode and heapify all
    // internal modes in bottom up way
    for (i = (maxHeap->size - 2) / 2; i >= 0; --i)
        maxHeapify(maxHeap, i);
    return maxHeap;
}

// The main function to sort an array of given size
void heapSort(int* array, int size)
{
    // Build a heap from the input data.
    struct MaxHeap* maxHeap = createAndBuildHeap(array, size);

    // Repeat following steps while heap size is greater than 1.
    // The last element in max heap will be the minimum element
    while (maxHeap->size > 1)
    {
        // The largest item in Heap is stored at the root. Replace
        // it with the last item of the heap followed by reducing the
        // size of heap by 1.
        swap(&maxHeap->array[0], &maxHeap->array[maxHeap->size - 1]);
        --maxHeap->size;  // Reduce heap size

        // Finally, heapify the root of tree.
        maxHeapify(maxHeap, 0);
    }
}

void divideArray(int* array, int size){
    firstPart= malloc(sizeof(int) * (SIZE / 5));
    secondPart = malloc(sizeof(int) * (SIZE / 5));
    thirdPart = malloc(sizeof(int) * (SIZE / 5));
    fourthPart = malloc(sizeof(int) * (SIZE / 5));
    fifthPart = malloc(sizeof(int) * (SIZE / 5));

    memcpy(firstPart, array, sizeof(int) * (SIZE / 5));
    memcpy(secondPart, array + (SIZE / 5), sizeof(int) * (SIZE / 5));
    memcpy(thirdPart, array + 2 * (SIZE / 5), sizeof(int) * (SIZE / 5));
    memcpy(fourthPart, array + 3 * (SIZE / 5), sizeof(int) * (SIZE / 5));
    memcpy(fifthPart, array + 4 * (SIZE / 5), sizeof(int) * (SIZE / 5));
}
/* GLOBAL DEGISKENLER */

// Thread sayisini global olarak tutacagiz
int thread_count;

int *results;

int size;
int *p, t;
int * getRandomArray( ) {

   static int  r[SIZE];
   int i;
   /* set the seed */
   srand( (unsigned)time( NULL ) );
  
   for ( i = 0; i < SIZE; i++) {
      r[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
      //printf( "r[%d] = %d\n", i, r[i]);
   }
   return r;
}

// Thread'lerin calistiracagi fonksiyon
void *runner() {
    pthread_mutex_lock(&mutex);
    if(count == 0){
    	divideArray(p, SIZE);
	heapSort(p, SIZE);
    }
    if(count == 1){
	heapSort(firstPart, (SIZE / 5));
    }
    if(count == 2){
	heapSort(secondPart, (SIZE / 5));
    }
    if(count == 3){
	heapSort(thirdPart, (SIZE / 5));
    }
    if(count == 4){
	heapSort(fifthPart, (SIZE / 5));
    }
    count++;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, const char *argv[])
{
    int i;

    p = getRandomArray();
		
    int err;
    pthread_t thread_id;
    if (argc != 3) {
        fprintf(stderr, "usage: %s <total points> <threads>\n", argv[0]);
        exit(1);
    }
    
    int argv_1 = atoi(argv[1]);

    pthread_t *threads; //yaratilacak threadlerin tutulacagi dinamik dizi

    // Global degiskenler
    thread_count = 5;

    // Her elemani bir pthread_t gostericisi olan, thread_count elemanli bir bellek alani

    threads = (pthread_t *)malloc(sizeof(pthread_t)*thread_count);
    if(threads == NULL) {
	exit(1);
    }

    // Her threadin sonucu kaydedecegi global results gostericisine malloc ile yer ayirdik
    results = (int*)malloc(sizeof(int));



    // thread_count adet thread yaratir
    for (i = 0; i < thread_count; i++) {
    err = pthread_create(&threads[i], NULL,  runner, NULL);
    if(err != 0) {
    fprintf(stderr,"pthread create failed\n");
    exit(1);
    }
    }

    // ebeveyn surec yarattigi thread'leri pthread_join() ile bekliyor.

    for (i = 0; i < thread_count; i++) {
    err = pthread_join(threads[i],NULL);
    if(err != 0) {
    fprintf(stderr,"pthread_join failed\n");
    exit(1);
    }
    }
    for ( int i = 0; i < SIZE; i++ ) {
        printf( "*(p + %d) : %d\n", i, *(p + i));
    }
    for ( int i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(firstPart + %d) : %d\n", i, *(firstPart + i));
    }
    for ( int i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(secondPart + %d) : %d\n", i, *(secondPart + i));
    }
    for ( int i = 0; i < SIZE / 5; i++ ) {
        printf( "*(thirdPart + %d) : %d\n", i, *(thirdPart + i));
    }
    for ( int i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(fourthPart + %d) : %d\n", i, *(fourthPart + i));
    }
    for ( int i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(fifthPart + %d) : %d\n", i, *(fifthPart + i));
    }

    //Garbage collector
    free(threads);
    free(results);
    return 0;
}

