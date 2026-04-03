import threading
import time
import random
import os

PROC_PATH = "/proc/sorted_queue"

NUM_WRITERS = 5
NUM_READERS = 5
OPERATIONS_PER_THREAD = 200

lock = threading.Lock()
written = []
read = []

def writer(thread_id):
    for i in range(OPERATIONS_PER_THREAD):
        msg = f"T{thread_id}-{i}\n"
        try:
            with open(PROC_PATH, "w") as f:
                f.write(msg)
            with lock:
                written.append(msg.strip())
        except Exception as e:
            print("writer error:", e)

        time.sleep(random.uniform(0.001, 0.01))


def reader(thread_id):
    for _ in range(OPERATIONS_PER_THREAD):
        try:
            with open(PROC_PATH, "r") as f:
                data = f.read().strip()
                if data:
                    with lock:
                        read.append(data)
        except Exception as e:
            print("reader error:", e)

        time.sleep(random.uniform(0.001, 0.01))


threads = []

# start writers
for i in range(NUM_WRITERS):
    t = threading.Thread(target=writer, args=(i,))
    t.start()
    threads.append(t)

# start readers
for i in range(NUM_READERS):
    t = threading.Thread(target=reader, args=(i,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()

print("\nDONE")
print("Written:", len(written))
print("Read   :", len(read))