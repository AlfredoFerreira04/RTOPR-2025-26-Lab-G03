#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Node structure for the unbounded buffer (linked list)
typedef struct Node
{
    int data;
    struct Node *next;
} Node;

Node *head = NULL; // head of the buffer
Node *tail = NULL; // tail of the buffer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void print_buffer()
{
    Node *temp = head;
    printf("Buffer: [");
    while (temp != NULL)
    {
        printf("%d", temp->data);
        if (temp->next)
            printf(", ");
        temp = temp->next;
    }
    printf("]\n");
}

void *producer(void *arg)
{
    int id = *((int *)arg);
    int item;
    while (1)
    {
        item = rand() % 100; // produce an item
        Node *newNode = malloc(sizeof(Node));
        newNode->data = item;
        newNode->next = NULL;

        pthread_mutex_lock(&mutex);
        if (tail == NULL)
        { // empty buffer
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        printf("Producer %d produced %d\n", id, item);
        // print_buffer();             // print buffer after production
        pthread_cond_signal(&cond); // signal consumers
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
        while (head == NULL)
        { // buffer empty
            pthread_cond_wait(&cond, &mutex);
        }
        Node *temp = head;
        item = temp->data;
        head = head->next;
        if (head == NULL)
            tail = NULL; // list empty
        free(temp);
        printf("Consumer %d consumed %d\n", id, item);
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