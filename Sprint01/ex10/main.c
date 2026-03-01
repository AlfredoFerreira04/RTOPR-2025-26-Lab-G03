#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define CLIENT_COUNT 100

typedef struct
{
    int id;
    char name[20];
    double balance;
} Client;

Client clients[CLIENT_COUNT];
Client negativeClients[CLIENT_COUNT];
int negativeCount = 0;

double averageBalance = 0.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_negative_ready = PTHREAD_COND_INITIALIZER;

int negative_ready = 0;

/* Generate random clients */
void generateClients()
{
    srand(time(NULL));
    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        clients[i].id = i + 1;
        sprintf(clients[i].name, "Client-%d", i + 1);
        clients[i].balance = ((double)rand() / RAND_MAX) * 2000 - 1000; // Range: -1000 to +1000
    }
}

/* Thread 1: Detect negative balances */
void *detectNegative(void *arg)
{
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].balance < 0)
        {
            negativeClients[negativeCount++] = clients[i];
        }
    }

    negative_ready = 1;
    pthread_cond_signal(&cond_negative_ready);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

/* Thread 2: Print negative clients */
void *printNegative(void *arg)
{
    pthread_mutex_lock(&mutex);

    while (!negative_ready)
        pthread_cond_wait(&cond_negative_ready, &mutex);

    if (negativeCount > 0)
    {
        printf("\nClients with negative balance:\n");
        for (int i = 0; i < negativeCount; i++)
        {
            printf("ID: %d | Name: %s | Balance: %.2f\n", negativeClients[i].id, negativeClients[i].name, negativeClients[i].balance);
        }
    }
    else
    {
        printf("\nNo clients with negative balance.\n");
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* Thread 3: Compute average balance */
void *computeAverage(void *arg)
{
    double sum = 0;

    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        sum += clients[i].balance;
    }

    averageBalance = sum / CLIENT_COUNT;
    printf("\nAverage balance: %.2f\n", averageBalance);

    return NULL;
}

/* Thread 4 (Conditional): Eliminate negative balances */
void *eliminateNegative(void *arg)
{
    for (int i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].balance < 0)
        {
            clients[i].balance = 0;
        }
    }
    printf("\nNegative balances eliminated.\n");
    return NULL;
}

int main()
{
    pthread_t t1, t2, t3, t4;

    generateClients();

    /* Create first three threads */
    pthread_create(&t1, NULL, detectNegative, NULL);
    pthread_create(&t2, NULL, printNegative, NULL);
    pthread_create(&t3, NULL, computeAverage, NULL);

    /* Wait for them to finish */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    /* Conditional behavior */
    if (averageBalance < 0)
    {
        pthread_create(&t4, NULL, eliminateNegative, NULL);
        pthread_join(t4, NULL);
    }
    else
    {
        printf("\nAll balances are in conformity. Exiting.\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_negative_ready);

    return 0;
}