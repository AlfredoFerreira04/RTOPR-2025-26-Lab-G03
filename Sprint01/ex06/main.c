#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    pid_t pid;
    int status;
    int random_number;

    // First child process
    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0)
    {
        // Child process
        srand(time(NULL) ^ getpid());
        random_number = rand() % 6; // Random number generation

        printf("First child generated: %d\n", random_number);
        exit(random_number); // Return number via exit status
    }
    else
    {
        // Parent process
        wait(&status); // wait for first child

        if (WIFEXITED(status))
        {
            random_number = WEXITSTATUS(status);
        }
        else
        {
            printf("Child did not exit normally.\n");
            exit(1);
        }

        printf("Parent received: %d\n", random_number);

        // Create random_number of child processes
        for (int i = 0; i < random_number; i++)
        {
            pid_t child_pid = fork();

            if (child_pid == 0)
            {
                printf("Child process #%d (PID: %d) created\n", i + 1, getpid());
                exit(0);
            }
        }

        // Wait for all newly created children
        for (int i = 0; i < random_number; i++)
        {
            wait(NULL);
        }

        printf("Parent exiting.\n");
    }

    return 0;
}