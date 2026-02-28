# Exercise 5

Developed code: ![code file](main.c)

## A

The multi process approach differs from the single process approach in that it divides the work among several child processes that run concurrently, while the single process approach performs all computations sequentially within one process.
Using multiple processes can take advantage of multicore CPUs and potentially reduce execution time for large datasets. However, it also introduces overhead from process creation, context switching, and process communication. For a small arrays, a single process approach is generally more advantageous because the overhead of creating and managing multiple processes is likely greater than the time required to compute the result sequentially.
