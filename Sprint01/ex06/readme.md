# Exercise 6

## Context

Write a program that does the following:

- the parent process creates a new child process;
- the new child process generates a random number between 0 and
5 and returns that value to the parent;
- upon reading the return value, the parent process should create
that number of child processes;
- each child process should print the number of its creation and exit;
- the parent should wait for all child processes and exit.

## Solution

See file: [main.c](main.c)
