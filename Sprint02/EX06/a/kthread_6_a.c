#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/kthread.h>      
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/atomic.h>
#include<linux/workqueue.h>
#include <linux/random.h>
#include <linux/proc_fs.h>
#define NTASKS 2

static atomic_t number;

int  proc_init(void);
void proc_exit(void);
int kth_function(void *pv);
struct task_struct *kth1, *kth2;
int th_id1 = 1, th_id2 = 2;

static struct workqueue_struct *wq;

typedef struct task {
    struct work_struct work;
    u32 flag; // 0 for increment, 1 for decrement
} task_t;

struct hrtimer timer;
ktime_t interval;
unsigned long timer_interval_ns = NSEC_PER_SEC; //1 seconds

/*
** Thread Function
*/
void task_function(struct work_struct *work) {
    task_t *task = container_of(work, task_t, work);

    if(task->flag == 0){
        atomic_inc(&number);
    }else{
        atomic_dec(&number);
    }
    
    printk(KERN_INFO "Changed number to %d", number.counter);
    kfree(task); // was getting a memory leak
}

enum hrtimer_restart timer_callback( struct hrtimer *t)
{
  	ktime_t currtime, interval;
  	currtime  = ktime_get();
	
  	interval = ktime_set(0,timer_interval_ns); 
  	hrtimer_forward(t, currtime, interval);

    if (wq) {
    task_t *tasks[NTASKS];
        for(int i = 0; i < NTASKS; i++){
            tasks[i] = kmalloc(sizeof(task_t), GFP_KERNEL);
            if (tasks[i]) {
                INIT_WORK(&tasks[i]->work, task_function);
                tasks[i]->flag = get_random_u32_below(2);
                queue_work(wq, &tasks[i]->work);
            }
        }
    }

	return HRTIMER_RESTART;
}

int __init proc_init(void){
    atomic_set(&number, 0);
    ktime_t interval;

	interval = ktime_set(0,timer_interval_ns); 
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer.function = &timer_callback;
 	hrtimer_start(&timer, interval, HRTIMER_MODE_REL);

    wq = create_workqueue("LKM_WQ");
    
	return 0;
}
void __exit proc_exit(void)
{
    int ret;

    ret = hrtimer_cancel(&timer);

    if (ret)
        printk(KERN_INFO "Timer was active\n");
    else
        printk(KERN_INFO "Timer was not active\n");

    if (wq) {
        destroy_workqueue(wq);
    }

    printk(KERN_INFO "Module unloaded\n");
}
module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PBS");
MODULE_DESCRIPTION("The kernel thread example");



