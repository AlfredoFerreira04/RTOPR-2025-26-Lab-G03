#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define NR_WORKERS 10
#define NR_NUMBERS 1000
#define NUMBERS_CEIL 5000

int big_arr[NR_NUMBERS];

typedef struct
{
    int *arr_part;
    int size;
} thread_data;

// Thread function
void *find_local_max(void *arg)
{
    thread_data *data = (thread_data *)arg;

    int local_max = data->arr_part[0];

    for (int i = 1; i < data->size; i++)
    {
        if (data->arr_part[i] > local_max)
        {
            local_max = data->arr_part[i];
        }
    }

    printf("Thread %lu found local max: %d\n", (unsigned long)pthread_self(), local_max);

    int *result = malloc(sizeof(int));
    *result = local_max;

    free(data); // free thread data
    pthread_exit(result);
}

int main(void)
{
    pthread_t threads[NR_WORKERS];
    srand(time(NULL));

    // Fill array with random numbers
    for (int i = 0; i < NR_NUMBERS; i++)
    {
        big_arr[i] = rand() % NUMBERS_CEIL;
    }

    int chunk_size = NR_NUMBERS / NR_WORKERS;

    // Create threads
    for (int i = 0; i < NR_WORKERS; i++)
    {
        thread_data *data = malloc(sizeof(thread_data));
        data->arr_part = &big_arr[i * chunk_size];
        data->size = chunk_size;

        if (pthread_create(&threads[i], NULL, find_local_max, data) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
    }

    int global_max = 0;

    // Join threads and collect results
    for (int i = 0; i < NR_WORKERS; i++)
    {
        void *ret_val;
        pthread_join(threads[i], &ret_val);

        int local_max = *((int *)ret_val);

        if (local_max > global_max)
        {
            global_max = local_max;
        }

        free(ret_val); // free returned value
    }

    printf("The maximum value found is: %d\n", global_max);

    return 0;
}