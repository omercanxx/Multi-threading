#define _GNU_SOURCE
#define SIZE 100000
#define LOWER -10000
#define UPPER 10000

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sched.h>
#include <stdint.h>

int *firstPart, *secondPart, *thirdPart, *fourthPart,* fifthPart;

/* Zaman olcum fonksiyonu */
void time_it(struct timeval *t2, struct timeval *t1) {
  long int diff = 
    (t2->tv_usec + 1000000 * t2->tv_sec) - 
    (t1->tv_usec + 1000000 * t1->tv_sec);
    fprintf(stdout, "***TIME***\n%.6f milliseconds\n", diff / 1000.);
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/* HEAP SORT */
struct MaxHeap
{
    int size;
    int* array;
};


void swap(int* a, int* b) { int t = *a; *a = *b;  *b = t; }

void maxHeapify(struct MaxHeap* maxHeap, int idx)
{
    int largest = idx;
    int left = (idx << 1) + 1;  
    int right = (idx + 1) << 1; 

    if (left < maxHeap->size &&
        maxHeap->array[left] > maxHeap->array[largest])
        largest = left;

    if (right < maxHeap->size &&
        maxHeap->array[right] > maxHeap->array[largest])
        largest = right;

    if (largest != idx)
    {
        swap(&maxHeap->array[largest], &maxHeap->array[idx]);
        maxHeapify(maxHeap, largest);
    }
}

struct MaxHeap* createAndBuildHeap(int *array, int size)
{
    int i;
    struct MaxHeap* maxHeap =
              (struct MaxHeap*) malloc(sizeof(struct MaxHeap));
    maxHeap->size = size;   
    maxHeap->array = array; 

    for (i = (maxHeap->size - 2) / 2; i >= 0; --i)
        maxHeapify(maxHeap, i);
    return maxHeap;
}

void heapSort(int* array, int size)
{
    struct MaxHeap* maxHeap = createAndBuildHeap(array, size);

    while (maxHeap->size > 1)
    {
        swap(&maxHeap->array[0], &maxHeap->array[maxHeap->size - 1]);
        --maxHeap->size;

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

int count = 0;
int thread_count;
int *results;
int size;
int *p;
int *sorted;
int * getRandomArray( ) {

   static int  r[SIZE];
   int i;
   srand( (unsigned)time( NULL ) );
  
   for ( i = 0; i < SIZE; i++) {
      r[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
   }
   return r;
}

// Thread'lerin calistiracagi fonksiyon
void *runner() {
    pthread_mutex_lock(&mutex);
    if(count == 0){
    	divideArray(p, SIZE);
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
	heapSort(fourthPart, (SIZE / 5));
    }
    if(count == 5){
	heapSort(fifthPart, (SIZE / 5));
    }
    count++;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, const char *argv[])
{
    int argv_1 = atoi(argv[1]);
    int argv_2 = atoi(argv[2]);

    pthread_attr_t attr;

    int ret;

    // Mainin süre ölçümü için gerekli
    struct timeval start, stop;

    // Ölçüm başlıyor
    gettimeofday(&start, NULL);

    int i, j, t;

    p = getRandomArray();

    int err;    
    pthread_t thread_id;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <total points> <threads>\n", argv[0]);
        exit(1);
    }
    srand(time(NULL));
    

    pthread_t *threads; 

    // Global degiskenler
    thread_count = 5;

    // Her elemani bir pthread_t gostericisi olan, thread_count elemanli bir bellek alani
    threads = (pthread_t *)malloc(sizeof(pthread_t)*thread_count);
    if(threads == NULL) {
	exit(1);
    }

    // Her threadin sonucu kaydedecegi global results gostericisine malloc ile yer ayirdik
    results = (int*)malloc(sizeof(int));
    
    ret = pthread_attr_init(&attr);
    
    if(argv_1 == 1)
    {
	ret = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM);
	if(argv_2 == 1)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_OTHER);
	}
	else if(argv_2 == 2)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_BATCH);
	}
	else if(argv_2 == 3)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_IDLE);
	}
    }
    else if(argv_1 == 2)
    {
	ret = pthread_attr_setscope( &attr, PTHREAD_SCOPE_PROCESS);
	if(argv_2 == 1)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_OTHER);
	}
	else if(argv_2 == 2)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_BATCH);
	}
	else if(argv_2 == 3)
	{
	    ret = pthread_attr_setschedpolicy( &attr, SCHED_IDLE);
	}
    }
    else
    {
	fprintf(stderr, "ERROR\n");
    }
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

    /* Print
    for (i = 0; i < SIZE; i++ ) {
        printf( "*(p + %d) : %d\n", i, *(p + i));
    }
    for (i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(firstPart + %d) : %d\n", i, *(firstPart + i));
    }
    for (i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(secondPart + %d) : %d\n", i, *(secondPart + i));
    }
    for (i = 0; i < SIZE / 5; i++ ) {
        printf( "*(thirdPart + %d) : %d\n", i, *(thirdPart + i));
    }
    for (i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(fourthPart + %d) : %d\n", i, *(fourthPart + i));
    }
    for (i = 0; i < (SIZE / 5); i++ ) {
        printf( "*(fifthPart + %d) : %d\n", i, *(fifthPart + i));
    }*/


    //Concatenation
    int* concat = malloc(SIZE * sizeof(int));
    memcpy(concat, firstPart, (SIZE/5) * sizeof(int));
    memcpy(concat + (SIZE/5), secondPart, (SIZE/5) * sizeof(int));
    memcpy(concat + 2 * (SIZE/5), thirdPart, (SIZE/5) * sizeof(int));
    memcpy(concat + 3 * (SIZE/5), fourthPart, (SIZE/5) * sizeof(int));
    memcpy(concat + 4 * (SIZE/5), fifthPart, (SIZE/5) * sizeof(int));
    
    /* PRINT 
    for(i = 0; i < SIZE; i++){
    	printf("*(concat + %d ) : %d\n", i, *(concat+i));
    }
    */
    for(i = 0; i < SIZE - 1; i++)
    {
	for(j = i + 1; j < SIZE; j++)
	{
	    if(concat[i] > concat[j])
	    {
		t = concat[i];
		concat[i] = concat[j];
		concat[j] = t;
	    }
	}
    }
    /* PRINT
    for(i = 0; i < SIZE; i++){
    	printf("*(concat + %d ) : %d\n", i, *(concat+i));

    }
    */
    // Ölçüm bitti
    gettimeofday(&stop, NULL);
    time_it(&stop, &start);

    //Garbage collector
    free(threads);
    free(results);
    return 0;
}

