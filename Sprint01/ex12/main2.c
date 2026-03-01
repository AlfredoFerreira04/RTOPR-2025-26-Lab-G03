#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// Function to print buffer
void print_buffer()
{
    printf("Buffer: [");
    for (int i = 0; i < count; i++)
    {
        int index = (out + i) % BUFFER_SIZE;
        printf("%d", buffer[index]);
        if (i < count - 1)
            printf(", ");
    }
    printf("]\n");
}

void *producer(void *arg)
{
    int id = *((int *)arg);
    int item;
    while (1)
    {
        item = rand() % 100;
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE)
        {
            pthread_cond_wait(&not_full, &mutex);
        }
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Producer %d produced %d\n", id, item);
        // print_buffer(); // print buffer after production
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int id = *((int *)arg);
    int item;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (count == 0)
        {
            pthread_cond_wait(&not_empty, &mutex);
        }
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer %d consumed %d\n", id, item);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3);
    }
    return NULL;
}

int main()
{
    pthread_t prod[2], cons[2];
    int ids[2] = {1, 2};

    for (int i = 0; i < 2; i++)
    {
        pthread_create(&prod[i], NULL, producer, &ids[i]);
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_join(prod[i], NULL);
        pthread_join(cons[i], NULL);
    }
    return 0;
}