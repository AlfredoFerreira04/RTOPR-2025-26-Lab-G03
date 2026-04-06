#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/msr.h> // for rdtsc
#include <linux/sched.h>

#define ENTRY_NAME "tsc_value"

int proc_open(struct inode *inode, struct file *filp);
ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t proc_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int proc_close(struct inode *inode, struct file *filp);
int proc_init(void);
void proc_exit(void);

#define BUFFER_LEN 32
static char tsc_buffer[BUFFER_LEN];

int proc_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "LKM: %s:[%d] open\n", ENTRY_NAME, current->pid);

    return 0;
}
ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned long long tsc;
    int len = 0, ret = 0;

    if (*f_pos > 0)
        return 0;
    tsc = rdtsc();

    len = snprintf(tsc_buffer, BUFFER_LEN, "%llu\n", tsc);
    if (len <= 0)
        return -EFAULT;

    if (count < len)
        return -EFAULT;

    ret = raw_copy_to_user(buf, tsc_buffer, len);
    if (ret != 0)
        return -EFAULT;

    *f_pos += len;

    printk(KERN_INFO "LKM: %llu read by PID %d\n", tsc, current->pid);

    return len;
}

int proc_close(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "LKM: %s:[%d] close\n", ENTRY_NAME, current->pid);
    return 0;
}
static const struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_release = proc_close,
};
int proc_init(void)
{
    struct proc_dir_entry *proc_entry = NULL;
    proc_entry = proc_create(ENTRY_NAME, 0666, NULL, &proc_ops);
    if (proc_entry == NULL)
        return -ENOMEM;

    printk(KERN_INFO "LKM: /proc/%s created\n", ENTRY_NAME);
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(ENTRY_NAME, NULL);
    printk(KERN_INFO "LKM: Removing /proc/%s.\n", ENTRY_NAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RBA");
MODULE_DESCRIPTION("Time Stamp Counter (TSC) getter module");
