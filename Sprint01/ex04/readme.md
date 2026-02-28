# Exercise 4

## A

16 processes

## B

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void main()
{
    int i;
    int status;
    for (i = 0; i < 4; i++) {
        if (fork() == 0) {
            sleep(1); /*sleep(): unistd.h*/
            exit(0);
        }
    }
    printf("End of execution!\n");

    return 0;
}
```

## C

```c
void main()
{
    int i;
    int status;
    for (i = 0; i < 4; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            sleep(1); 
            exit(0);
        }       

        if((pid % 2) != 0){
            waitpid(pid,NULL, 0);
        }
    }
    printf("End of execution!\n");

    return 0;
}
```

## D

```c
void main()
{
    int i;
    int status;
    for (i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sleep(1);
            exit(i+1);
        }
        if((pid % 2) != 0){
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("Child returned: %d\n", WEXITSTATUS(status));
            }
        }
    }
    printf("End of execution!\n");
    return 0;
}
```
