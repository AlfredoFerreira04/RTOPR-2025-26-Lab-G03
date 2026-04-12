// Took an opportunity here to experiment with conditional compiling like was showed in later theoretical classes
// Depending on which flag is passed to the make configuration different options of concurrency/compilation will occur

// SPNLCK	- for spinlock bassed concurrency
// MUTEX	- for mutex based concurrency
// SEMPHR	- for semaphore based concurrency

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>

#define BUFFER_LEN	100
#define ENTRY_NAME "fifo_queue"

struct queue_item{ 
	char buffer[BUFFER_LEN];
	struct list_head node; 
}; 

static int pop (char *buffer);
static int push (char *buffer);


#ifdef SPNLCK
    #include <linux/spinlock.h>
    spinlock_t spin;

#elif defined(MUTEX)
    #include <linux/mutex.h>
    struct mutex mux;

#elif defined(SEMPHR)
    #include <linux/semaphore.h>
    struct semaphore sem;

#endif


#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
	void try_get_queue(void){
		#ifdef SPNLCK
			spin_lock(&spin);
		#elif MUTEX
			mutex_lock(&mux);
		#elif SEMPHR
			down(&sem);
		#endif
	}
	
	void release_queue(void){
		#ifdef SPNLCK
			spin_unlock(&spin);
		#elif MUTEX
			mutex_unlock(&mux);
		#elif SEMPHR
			up(&sem);
		#endif

	}
#endif



int proc_open(struct inode *inode, struct file *filp);
ssize_t  proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t  proc_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int proc_close(struct inode *inode, struct file *filp);
int proc_init(void);
void proc_exit(void);


struct list_head head; 

struct proc_dir_entry *proc_entry = NULL;

static int pop (char *buffer)
{
	struct queue_item *node_queue;
	
	#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
		try_get_queue();
	#endif

	if(list_empty(&head)){
		#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
			release_queue();
		#endif
		return 0;
	}
	node_queue = list_first_entry(&head,struct queue_item, node);
	strcpy(buffer, node_queue->buffer);
	list_del(&node_queue->node); 
	kfree(node_queue);

	#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
		release_queue();
	#endif

	return 1;
}

static int push (char *buffer)
{
	struct queue_item *node_queue = kmalloc(sizeof(struct queue_item),GFP_KERNEL);
	if(!node_queue)
		return 0;
	strcpy(node_queue->buffer,buffer);
	
	#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
		try_get_queue();
	#endif

	list_add_tail(&node_queue->node,&head); 
	
	#if defined(SPNLCK) || defined(MUTEX) || defined(SEMPHR)
		release_queue();
	#endif
	
	return 1;
}

int proc_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "LKM: %s:[%d] open\n",ENTRY_NAME, current->pid);
	return 0;
}

ssize_t  proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char buffer[BUFFER_LEN];
	int ret = 0, len = 0;
	printk(KERN_INFO "LKM: %s:[%d] read\n",ENTRY_NAME, current->pid);

	if(*f_pos > 0)
		return 0;
	if(!pop(buffer))
		return 0;
	len = strlen(buffer);
	if(len <= 0)
		return -EFAULT;
	if(count < len)
		return -EFAULT;
	ret=raw_copy_to_user(buf,buffer,len);
	if(ret != 0)
		return -EFAULT;
	*f_pos += count -len;
	return len;
}

ssize_t  proc_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	char buffer[BUFFER_LEN];
	printk(KERN_INFO "LKM: %s:[%d] write\n",ENTRY_NAME, current->pid);
	if (count > BUFFER_LEN)
        	return -EINVAL;
	ret = raw_copy_from_user(buffer, buf, count);
    	if (ret != 0)
        	return -EFAULT;
	buffer[count] = 0;
	if(!push(buffer))
		return -EFAULT;
	return count;
}

int proc_close(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "LKM: %s:[%d] close\n",ENTRY_NAME, current->pid);
	return 0; 
}

static const struct proc_ops proc_ops = {
 	.proc_open  	= proc_open,
 	.proc_read  	= proc_read,
	.proc_write  	= proc_write,
 	.proc_release  	= proc_close,
};

int proc_init(void)
{
	#ifdef SPNLCK
		spin_lock_init(&spin);
	#elif defined(MUTEX)
		mutex_init(&mux);
	#elif defined(SEMPHR)
		sema_init(&sem, 1);
	#endif

   	proc_entry = proc_create(ENTRY_NAME,0666, NULL, &proc_ops);
	if(proc_entry == NULL)
   		return -ENOMEM;
  
   	printk(KERN_INFO "LKM: /proc/%s created\n", ENTRY_NAME);
	INIT_LIST_HEAD(&head); 
   	return 0;
}

void proc_exit(void)
{
	struct queue_item *tmp;
	struct list_head *pos, *q;
   	remove_proc_entry(ENTRY_NAME, NULL);
   	printk(KERN_INFO "LKM: /proc/%s removed\n", ENTRY_NAME);
	list_for_each_safe(pos, q, &head){
		 tmp = list_entry(pos, struct queue_item, node);
		 list_del(pos);
		 kfree(tmp);
	}
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PBS");
MODULE_DESCRIPTION("FIFO queue implementation");
