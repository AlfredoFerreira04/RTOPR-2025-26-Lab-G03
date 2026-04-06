# Exercise 3

The purpose of this exercise if change the ring_buffer module in order to assure that a writing process is put in the waiting state if there is no space available in the buffer and resume its execution when there is such space. Similarly, for reader
processes that must be put in a waiting state if there is no data in the
buffer. Resume its execution when some data is put in such buffer.

Two resolutions must be done:

- [(A)](./a/ring_buffer.c) : Must be implemented using waitqueues
- [(B)](./b/ring_buffer.c) : Must be implemented using semaphores

## Evidence of module working properly

### A

- 1st Case: Write until buffer is full leads to writing process having to wait

![Wait Write](./a/evidence/Captura%20de%20ecrã%202026-04-06%20151257.png)

- 2nd Case: Buffer is read realising the writing process to proceed with its execution

![Wait Write Proceed Execution](./a/evidence/Captura%20de%20ecrã%202026-04-06%20151327.png)

- 2nd Case: Read until buffer is empty leads to reading process having to wait

![Wait Read](./a/evidence/Captura%20de%20ecrã%202026-04-06%20151412.png)

- 3rd Case: Buffer is written in realising the reading process to proceed with its execution

![Wait Read Proceed Execution](./a/evidence/Captura%20de%20ecrã%202026-04-06%20151441.png)

### B

- 1st Case: Write until buffer is full leads to writing process having to wait

![Wait Write](./b/evidence/Captura%20de%20ecrã%202026-04-06%20152908.png)

- 2nd Case: Buffer is read realising the writing process to proceed with its execution

![Wait Write Proceed Execution](./b/evidence/Captura%20de%20ecrã%202026-04-06%20152951.png)

- 2nd Case: Read until buffer is empty leads to reading process having to wait

![Wait Read](./b/evidence/Captura%20de%20ecrã%202026-04-06%20153111.png)

- 3rd Case: Buffer is written in realising the reading process to proceed with its execution

![Wait Read Proceed Execution](./b/evidence/Captura%20de%20ecrã%202026-04-06%20153139.png)
