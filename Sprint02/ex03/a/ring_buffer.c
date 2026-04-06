#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/wait.h>

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
};

static int increment(int *item);
static int is_empty(int r, int w);
static int is_full(int r, int w);
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
wait_queue_head_t read_queue;
wait_queue_head_t write_queue;

static int increment(int *item)
{
    int ret;
    ret = *item;
    *item = (*item + 1) % QUEUE_LEN;
    return ret;
}
static int is_empty(int r, int w)
{
    int ret;
    ret = !(r ^ w); // xor
    return ret;
}
static int is_full(int r, int w)
{
    int ret, write;
    write = (w + 1) % QUEUE_LEN;
    ret = (write == r);
    return ret;
}
static int dequeue(char *buffer)
{
    // Wait until buffer is NOT empty
    printk(KERN_INFO "LKM: Reader [%d] trying to read\n", current->pid);

    if (wait_event_interruptible(read_queue,
                                 !is_empty(ring.read_item, ring.write_item)))
    {
        printk(KERN_INFO "LKM: Reader [%d] interrupted\n", current->pid);
        return -ERESTARTSYS;
    }

    printk(KERN_INFO "LKM: Reader [%d] woke up and reading\n", current->pid);

    strcpy(buffer, ring.queue[ring.read_item].buffer);
    increment(&ring.read_item);

    wake_up_interruptible(&write_queue); // Wake up writers (space available now)
    return 1;
}
static int enqueue(char *buffer)
{
    printk(KERN_INFO "LKM: Writer [%d] trying to write\n", current->pid);

    // Wait until buffer is NOT full
    if (wait_event_interruptible(write_queue,
                                 !is_full(ring.read_item, ring.write_item)))
    {
        printk(KERN_INFO "LKM: Writer [%d] interrupted\n", current->pid);
        return -ERESTARTSYS;
    }

    printk(KERN_INFO "LKM: Writer [%d] woke up and writing\n", current->pid);

    strcpy(ring.queue[ring.write_item].buffer, buffer);
    increment(&ring.write_item);

    wake_up_interruptible(&read_queue); // Wake up readers (data available now)

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
    init_waitqueue_head(&read_queue);
    init_waitqueue_head(&write_queue);

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
