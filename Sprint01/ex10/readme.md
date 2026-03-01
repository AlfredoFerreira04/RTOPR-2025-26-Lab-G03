# Exercise 10

## Context

Develop a program that performs the following actions:
3

- creates an array of 100 clients;
- each client is identified by its own number, name and current
balance (values are randomly generated);
- creates three threads where:
  - one of the threads verifies if any of the clients has a negative
balance and signal each of these;
  - another thread prints the information of the clients with negative balance using the information from the previous thread;
  - another thread computes the average balance of all clients.
- If the average balance is negative, then another thread must be
created to eliminate the negative values;
- If the average balance is positive, then the main thread should
print a statement of conformity and exit.

## Solution

See file: [main.c](main.c)
