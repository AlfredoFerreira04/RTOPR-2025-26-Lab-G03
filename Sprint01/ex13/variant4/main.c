#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock      = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  can_read  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  can_write = PTHREAD_COND_INITIALIZER;

int readers_active  = 0;
int writers_active  = 0;
int readers_waiting = 0;
int writers_waiting = 0;

// já não usava um enum há muito tempo, mas é melhor que um numero mágico aí a flutuar
enum Last{
    WRITER,
    READER
};
enum Last last = 0;

int shared_data = 0;

#define NUM_READERS 6
#define NUM_WRITERS 6

// scheduling/dispatching helper porque as thread funcs estavam a ficar cheias de codigo repetido
static void dispatch(void) {
    int wake_writer = 0;
    int wake_reader = 0;

    if (writers_waiting > 0 && readers_waiting > 0) {

        if (last)
            wake_reader = WRITER;
        else
            wake_writer = 1;
    } else if (writers_waiting > 0) {
        wake_writer = 1;
    } else if (readers_waiting > 0) {
        wake_reader = 1;
    }

    if (wake_writer) {
        pthread_cond_signal(&can_write);
    } else if (wake_reader) {
        pthread_cond_broadcast(&can_read);
    }
}

void *writer(void *arg) {
    int id = *(int *)arg;

    usleep(rand() % 300000);

    printf("[Writer %d] Wants to write...\n", id);

    pthread_mutex_lock(&lock);
        writers_waiting++;
        while (readers_active > 0 || writers_active > 0)
            pthread_cond_wait(&can_write, &lock);
        writers_waiting--;
        writers_active++;
        last = WRITER;  // record for next assignment
    pthread_mutex_unlock(&lock);

    shared_data++;
    printf("[Writer %d] Writing... shared_data = %d\n", id, shared_data);
    usleep(200000);

    pthread_mutex_lock(&lock);
        writers_active--;
        dispatch();
    pthread_mutex_unlock(&lock);

    printf("[Writer %d] Done writing.\n", id);
    return NULL;
}

void *reader(void *arg) {
    int id = *(int *)arg;

    usleep(rand() % 300000);

    printf("[Reader %d] Wants to read...\n", id);

    pthread_mutex_lock(&lock);
        readers_waiting++;
        while (writers_active > 0 ||
              (writers_waiting > 0 && last == READER))
            pthread_cond_wait(&can_read, &lock);
        readers_waiting--;
        readers_active++;
        last = READER; // record for next assignment
    pthread_mutex_unlock(&lock);

    printf("[Reader %d] Reading... shared_data = %d  (active readers: %d)\n",
           id, shared_data, readers_active);

    usleep(150000);

    pthread_mutex_lock(&lock);
        readers_active--;
        if (readers_active == 0)
            dispatch(); 
    pthread_mutex_unlock(&lock);

    printf("[Reader %d] Done reading.\n", id);
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    int       reader_ids[NUM_READERS];
    int       writer_ids[NUM_WRITERS];

    srand(42);

    printf("=== No-Starvation Reader-Writer (last_was_writer alternation) ===\n\n");

    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        if (pthread_create(&readers[i], NULL, reader, &reader_ids[i]) != 0) {
            perror("Failed to create reader thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        if (pthread_create(&writers[i], NULL, writer, &writer_ids[i]) != 0) {
            perror("Failed to create writer thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_READERS; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++)
        pthread_join(writers[i], NULL);

    printf("\n=== All threads finished. Final shared_data = %d ===\n", shared_data);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&can_read);
    pthread_cond_destroy(&can_write);

    return 0;
}