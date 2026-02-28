#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 16
#define FILL_THREADS 2
#define CALC_THREADS 8

int A[SIZE][SIZE];
int B[SIZE][SIZE];
int C[SIZE][SIZE];

void* fill_matrix(void* arg) {
    int (*matrix)[SIZE] = (int (*)[SIZE])arg;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = rand() % 10; // Keeping numbers relatively small for readability
        }
    }
    return NULL;
}

void* multiply_rows(void* arg) {
    int thread_id = *(int*)arg;
    int rows_per_thread = SIZE / CALC_THREADS;
    int start_row = thread_id * rows_per_thread;
    int end_row = start_row + rows_per_thread;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < SIZE; j++) {
            C[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    free(arg);
    return NULL;
}

int main() {
    pthread_t fillers[FILL_THREADS];
    pthread_t workers[CALC_THREADS];
    srand(time(NULL));

    pthread_create(&fillers[0], NULL, fill_matrix, (void*)A);
    pthread_create(&fillers[1], NULL, fill_matrix, (void*)B);

    // need to wait for fillers to finish
    for (int i = 0; i < FILL_THREADS; i++) pthread_join(fillers[i], NULL);

    for (int i = 0; i < CALC_THREADS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&workers[i], NULL, multiply_rows, id);
    }

    for (int i = 0; i < CALC_THREADS; i++) pthread_join(workers[i], NULL);

    printf("Resulting 16x16 Matrix C:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%4d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}