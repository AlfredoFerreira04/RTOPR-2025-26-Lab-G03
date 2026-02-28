#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define NR_WORKERS          10
#define NR_NUMBERS          1000 // 1000 positions as per requirements
#define NUMBERS_CEIL        5000       

int big_arr[NR_NUMBERS];
int small_arr[NR_WORKERS];
pthread_mutex_t lock; // Mutex for synchronization

// Thread function
void* find_local_max(void* arg) {
    int id = *((int*)arg);
    int start = (id - 1) * (NR_NUMBERS / NR_WORKERS);
    int end = start + (NR_NUMBERS / NR_WORKERS);
    int local_max = 0;

    for (int i = start; i < end; i++) {
        if (big_arr[i] > local_max) {
            local_max = big_arr[i];
        }
    }

    pthread_mutex_lock(&lock);
    small_arr[id - 1] = local_max;
    pthread_mutex_unlock(&lock);

    free(arg);
    return NULL;
}

int main(void) {
    pthread_t threads[NR_WORKERS];
    pthread_mutex_init(&lock, NULL);
    srand(time(NULL));

    for (int i = 0; i < NR_NUMBERS; i++) {
        big_arr[i] = rand() % NUMBERS_CEIL;
    }

    for (int i = 0; i < NR_WORKERS; i++) {
        int* id = malloc(sizeof(int)); 
        *id = i + 1;
        if (pthread_create(&threads[i], NULL, find_local_max, id) != 0) {
            perror("Failed to create thread");
        }
    }

    for (int i = 0; i < NR_WORKERS; i++) {
        pthread_join(threads[i], NULL);
    }

    int global_max = 0;
    for (int i = 0; i < NR_WORKERS; i++) {
        if (small_arr[i] > global_max) {
            global_max = small_arr[i];
        }
    }

    printf("The maximum value found is: %d\n", global_max);

    pthread_mutex_destroy(&lock);
    return 0;
}