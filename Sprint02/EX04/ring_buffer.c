#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/string.h>

#define ENTRY_NAME "ring_buffer"
#define BUFFER_LEN 100
#define QUEUE_LEN  5

struct queue_item{ 
	char buffer[BUFFER_LEN];
}; 

struct ring{
	struct queue_item queue[QUEUE_LEN];
	int write_item;
	int read_item;
};

static struct ring ring;
static struct proc_dir_entry *proc_entry = NULL;

static struct timer_list my_timer;
static char last_msg[BUFFER_LEN] = "EMPTY\n";

static int increment(int *item)
{
	int ret = *item;
	*item = (*item + 1) % QUEUE_LEN;
	return ret;
}

static int is_empty(int r, int w)
{
	return !(r ^ w);
}

static int is_full(int r, int w)
{
	return ((w + 1) % QUEUE_LEN) == r;
}

static int dequeue(char *buffer)
{
	if (!is_empty(ring.read_item, ring.write_item)) {
		strcpy(buffer, ring.queue[ring.read_item].buffer);
		increment(&ring.read_item);
		return 1;
	}
	return 0;
}

static int enqueue(char *buffer)
{
	if (is_full(ring.read_item, ring.write_item))
		increment(&ring.read_item); // overwrite oldest

	strcpy(ring.queue[ring.write_item].buffer, buffer);
	increment(&ring.write_item);
	return 1;
}

static void timer_callback(struct timer_list *t)
{
	char buffer[BUFFER_LEN];

	if (dequeue(buffer)) {
		strcpy(last_msg, buffer);
		printk(KERN_INFO "Dequeued: %s\n", buffer);
	} else {
		printk(KERN_INFO "Last message: %s\n", last_msg);
	}

	mod_timer(&my_timer, jiffies + HZ);
}

int proc_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "LKM: %s:[%d] open\n", ENTRY_NAME, current->pid);
	return 0;
}

ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int len;

	printk(KERN_INFO "LKM: %s:[%d] read\n", ENTRY_NAME, current->pid);

	if (*f_pos > 0)
		return 0;

	len = strlen(last_msg);

	if (count < len)
		return -EINVAL;

	if (raw_copy_to_user(buf, last_msg, len))
		return -EFAULT;

	*f_pos = len;
	return len;
}

ssize_t proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	char buffer[BUFFER_LEN];

	printk(KERN_INFO "LKM: %s:[%d] write\n", ENTRY_NAME, current->pid);

	if (count >= BUFFER_LEN)
		return -EINVAL;

	if (raw_copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	enqueue(buffer);

	return count;
}

int proc_close(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "LKM: %s:[%d] close\n", ENTRY_NAME, current->pid);
	return 0; 
}

static const struct proc_ops proc_ops = {
	.proc_open    = proc_open,
	.proc_read    = proc_read,
	.proc_write   = proc_write,
	.proc_release = proc_close,
};


int proc_init(void)
{
	proc_entry = proc_create(ENTRY_NAME, 0666, NULL, &proc_ops);
	if (proc_entry == NULL)
		return -ENOMEM;

	printk(KERN_INFO "LKM: /proc/%s created\n", ENTRY_NAME);

	ring.write_item = 0;
	ring.read_item  = 0;

	/* 🔹 setup timer */
	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + HZ);

	return 0;
}

void proc_exit(void)
{
	del_timer_sync(&my_timer);
	remove_proc_entry(ENTRY_NAME, NULL);

	printk(KERN_INFO "LKM: /proc/%s removed\n", ENTRY_NAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PBS");
MODULE_DESCRIPTION("Ring buffer with timer-based dequeue");