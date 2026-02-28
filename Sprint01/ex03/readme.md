# Exercise 3

## A

In parent process:

* a=5
* b=pid of the child process
* c=pid of the current process
* d=the pid of the parent process (pid 1 init if it was not created by fork())

In child process:

* a=5
* b=0
* c=pid of the current process
* d=the pid of the parent process

## B

The value return from getpid() when called in the parent process is the same as the return value from getppid() when called in the child process.
The value of "a" is the same in both, child and parent processes since separate memory spaces.
