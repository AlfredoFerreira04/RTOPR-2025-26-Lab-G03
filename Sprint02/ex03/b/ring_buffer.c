#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>

#define ENTRY_NAME "ring_buffer_mods"
#define BUFFER_LEN 100
#define QUEUE_LEN 5

struct queue_item
{
    char buffer[BUFFER_LEN];
};

struct ring
{
    struct queue_item queue[QUEUE_LEN];
    int write_item;
    int read_item;
    struct semaphore sem_empty; // Counts empty slots for writers
    struct semaphore sem_full;  // Counts filled slots for readers
    struct semaphore sem_mutex; // Mutual exclusion for buffer access
};

static int increment(int *item);
static int enqueue(char *buffer);
static int dequeue(char *buffer);

int proc_open(struct inode *inode, struct file *filp);
ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t proc_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int proc_close(struct inode *inode, struct file *filp);

int proc_init(void);
void proc_exit(void);

struct ring ring;

struct proc_dir_entry *proc_entry = NULL;

static int increment(int *item)
{
    int ret;
    ret = *item;
    *item = (*item + 1) % QUEUE_LEN;
    return ret;
}

static int dequeue(char *buffer)
{
    printk(KERN_INFO "LKM: %s:[%d] trying to read\n", ENTRY_NAME, current->pid);

    // Wait for available data
    if (down_interruptible(&ring.sem_full))
    {
        printk(KERN_INFO "LKM: %s:[%d] read interrupted\n", ENTRY_NAME, current->pid);
        return -ERESTARTSYS;
    }

    printk(KERN_INFO "LKM: %s:[%d] got data, reading...\n", ENTRY_NAME, current->pid);

    // Lock buffer access
    down(&ring.sem_mutex);

    strcpy(buffer, ring.queue[ring.read_item].buffer);
    increment(&ring.read_item);

    // Unlock buffer access
    up(&ring.sem_mutex);

    // Signal that there is an empty slot for writers
    up(&ring.sem_empty);

    printk(KERN_INFO "LKM: %s:[%d] read done\n", ENTRY_NAME, current->pid);
    return 1;
}
static int enqueue(char *buffer)
{
    printk(KERN_INFO "LKM: %s:[%d] trying to write\n", ENTRY_NAME, current->pid);

    // Wait for empty slot
    if (down_interruptible(&ring.sem_empty))
    {
        printk(KERN_INFO "LKM: %s:[%d] write interrupted\n", ENTRY_NAME, current->pid);
        return -ERESTARTSYS;
    }

    printk(KERN_INFO "LKM: %s:[%d] got empty slot, writing...\n", ENTRY_NAME, current->pid);

    // Lock buffer access
    down(&ring.sem_mutex);

    strcpy(ring.queue[ring.write_item].buffer, buffer);
    increment(&ring.write_item);

    // Unlock buffer access
    up(&ring.sem_mutex);

    // Signal that there is a new item for readers
    up(&ring.sem_full);

    printk(KERN_INFO "LKM: %s:[%d] write done\n", ENTRY_NAME, current->pid);
    return 1;
}

int proc_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "LKM: %s:[%d] open\n", ENTRY_NAME, current->pid);
    return 0;
}
ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    char buffer[BUFFER_LEN];
    int ret = 0, len = 0;
    printk(KERN_INFO "LKM: %s:[%d] read\n", ENTRY_NAME, current->pid);

    if (*f_pos > len)
    {
        *f_pos = 0;
        return 0;
    }
    ret = dequeue(buffer);
    if (ret <= 0)
        return ret;
    len = strlen(buffer);
    if (len <= 0)
        return -EFAULT;
    if (count < len)
        return -EFAULT;
    ret = raw_copy_to_user(buf, buffer, len);

    if (ret != 0)
        return -EFAULT;
    *f_pos += count - len;
    return len;
}
ssize_t proc_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    int ret;
    char buffer[BUFFER_LEN];
    printk(KERN_INFO "LKM: %s:[%d] write\n", ENTRY_NAME, current->pid);
    if (count > BUFFER_LEN)
        return -EINVAL;
    ret = raw_copy_from_user(buffer, buf, count);
    if (ret != 0)
        return -EFAULT;
    buffer[count] = 0;
    ret = enqueue(buffer);
    if (ret <= 0)
        return ret;
    return count;
}

int proc_close(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "LKM: %s:[%d] close\n", ENTRY_NAME, current->pid);
    return 0;
}
static const struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_write = proc_write,
    .proc_release = proc_close,
};
int proc_init(void)
{
    proc_entry = proc_create(ENTRY_NAME, 0666, NULL, &proc_ops);
    if (proc_entry == NULL)
        return -ENOMEM;

    printk(KERN_INFO "LKM: /proc/%s created\n", ENTRY_NAME);

    ring.write_item = 0;
    ring.read_item = 0;
    sema_init(&ring.sem_empty, QUEUE_LEN); // All slots initially empty
    sema_init(&ring.sem_full, 0);          // No data initially
    sema_init(&ring.sem_mutex, 1);         // Mutex starts unlocked
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(ENTRY_NAME, NULL);
    printk(KERN_INFO "LKM: /proc/%s removed\n", ENTRY_NAME);
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PBS/RBA");
MODULE_DESCRIPTION("Ring buffer with modifications implementation");
