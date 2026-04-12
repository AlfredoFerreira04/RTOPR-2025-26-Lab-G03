import time
import threading

PROC_FILE = "/proc/ring_buffer"

WRITE_COUNT = 20
READ_INTERVAL = 0.5  # seconds


def writer():
    for i in range(WRITE_COUNT):
        msg = f"msg_{i}\n"
        try:
            with open(PROC_FILE, "w") as f:
                f.write(msg)
            print(f"[WRITE] {msg.strip()}")
        except Exception as e:
            print(f"writer error: {e}")
        time.sleep(0.1)  # fast writes


def reader():
    while True:
        try:
            with open(PROC_FILE, "r") as f:
                data = f.read()
            print(f"[READ ] {data.strip()}")
        except Exception as e:
            print(f"reader error: {e}")
        time.sleep(READ_INTERVAL)


if __name__ == "__main__":
    t_w = threading.Thread(target=writer)
    t_r = threading.Thread(target=reader, daemon=True)

    t_r.start()
    t_w.start()

    t_w.join()

    print("Writer finished. Watching reads for 5 seconds...\n")
    time.sleep(5)