# Exercise 1

## A

After the call to fork(), two processes execute concurrently:

* Child process (p == 0)
* Parent process (p > 0, where p equals the child’s PID)
  
In the parent process:

```c
x = x + 2  →  x = 2
printf("Step 1. x = 2");
printf("Step 3. 0; x = 2");
```

In the child process:

```c
x = x - 2  →  x = -2
printf("Step 2. x = -2");
printf("Step 3. <child_pid>; x = -2");
```

## B

The order is not guaranteed because:

* After fork(), both processes run independently.
* The operating system scheduler decides which process executes first.

However, there are some guarantees:

* Each process executes its own Step 1/2 before its own Step 3.
* Therefore, Step 3 cannot appear before its corresponding Step 1 or Step 2.
* However, lines from parent and child may interleave in any order.
