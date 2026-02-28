#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NR_WORKERS 5
#define SIZE_ARR 500

int maxFromSection(int section, int *arr)
{
    int block_size = SIZE_ARR / NR_WORKERS;
    int start = block_size * section;
    int maximum = 0;
    for (int i = start; i < (start + block_size); i++)
    {
        if (*(arr + i) > maximum)
        {
            maximum = *(arr + i);
        }
    }

    // printf("Process responsible for section %d, started at %d maximum value %d\n", section, start, maximum);

    return maximum;
}

int main()
{
    srand(time(NULL));
    pid_t children[NR_WORKERS];
    int arr[SIZE_ARR];
    int sum = 0;

    for (int i = 0; i < SIZE_ARR; i++)
    {
        arr[i] = rand() % 255;
    }

    for (int i = 0; i < NR_WORKERS; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            exit(maxFromSection(i, arr));
        }
        else
        {
            children[i] = pid;
        }
    }

    for (int i = 0; i < NR_WORKERS; i++)
    {
        int status;
        waitpid(children[i], &status, 0);
        sum += WEXITSTATUS(status);
    }

    printf("Finale sum was: %d\n", sum);

    return 0;
}