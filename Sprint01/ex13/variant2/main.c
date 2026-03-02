#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NR_READERS 4
#define NR_WRITERS 3

typedef struct {
    uint8_t *arr;
    pthread_mutex_t mux;
    pthread_mutex_t read_inc_mux;
    int count;
} thread_struct;

void *writer_routine(void *args) {
    thread_struct *s = (thread_struct *)args;
    pthread_mutex_lock(&s->mux);

    uint8_t num    = rand() % 8;
    uint8_t fill   = rand() % 64;
    short offset   = rand() % 448 + fill;
    short start    = 512 - offset;
    printf("I AM WRITER!!!\n");
    for (int i = start; i < start + fill; i++)
        s->arr[i] = num;

    pthread_mutex_unlock(&s->mux);
    return NULL;
}

void *reader_routine(void *args) {
    thread_struct *s = (thread_struct *)args;
    

    pthread_mutex_lock(&s->read_inc_mux);
    s->count++;
    if(s->count == 1) pthread_mutex_lock(&s->mux);
    pthread_mutex_unlock(&s->read_inc_mux);

    printf("I am READER!!!\n");

    for (int i = 0; i < 512; i++)
        printf("%d ", s->arr[i]);
    
    printf("\nI AND ENDING READER!!\n"),
 
    pthread_mutex_lock(&s->read_inc_mux);
    s->count--;
    if(s->count == 0) pthread_mutex_unlock(&s->mux);
    pthread_mutex_unlock(&s->read_inc_mux);

    return NULL;
}

int main(void) {
    srand(time(NULL));

    thread_struct sync_struct;
    pthread_mutex_init(&sync_struct.mux, NULL);
    pthread_mutex_init(&sync_struct.read_inc_mux, NULL);
    sync_struct.arr = calloc(512, sizeof(uint8_t)); 

    sync_struct.count = 0;


    pthread_t readers[NR_READERS];  
    pthread_t writers[NR_WRITERS];

    for (int i = 0; i < NR_WRITERS; i++)
        if (pthread_create(&writers[i], NULL, writer_routine, &sync_struct) != 0)
            perror("Error creating writer thread");

    for (int i = 0; i < NR_READERS; i++)
        if (pthread_create(&readers[i], NULL, reader_routine, &sync_struct) != 0)
            perror("Error creating reader thread");

    for (int i = 0; i < NR_WRITERS; i++)
        pthread_join(writers[i], NULL);

    for (int i = 0; i < NR_READERS; i++)
        pthread_join(readers[i], NULL);

    free(sync_struct.arr);           

    pthread_mutex_destroy(&sync_struct.mux);
    pthread_mutex_destroy(&sync_struct.read_inc_mux);
    

    return 0;
}
