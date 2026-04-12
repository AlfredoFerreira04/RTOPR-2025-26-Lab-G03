#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/wait.h>

struct timer_list waking_timer;
wait_queue_head_t my_w_wq, my_r_wq;
struct task_struct *write_task, *read_task;
static int number = 0;
static int write_flag = 0;
static int read_flag = 0;


static void waking_callback(struct timer_list *t){
    wake_up(&my_w_wq);

    write_flag = 1;
    wake_up_interruptible(&my_w_wq);

    mod_timer(&waking_timer, jiffies + HZ);     // wakes in 1 sec
}

static int writing_kthread(void *arg){
    while(!kthread_should_stop()){
        wait_event_interruptible(my_w_wq, (write_flag == 1));

        number++;
        read_flag = 1;

        wake_up_interruptible(&my_r_wq);

        write_flag = 0;
    }

    return 0;
}

static int reading_kthread(void *arg){
    while(!kthread_should_stop()){
        wait_event_interruptible(my_r_wq, (read_flag == 1));
        printk(KERN_INFO "The number is %d", number);
        read_flag = 0;
    }

    return 0;
}

static int mod_init(void){
    init_waitqueue_head(&my_w_wq);
    init_waitqueue_head(&my_r_wq);

    write_task = kthread_run(writing_kthread, NULL, "WRITE_TASK");
    read_task = kthread_run(reading_kthread, NULL, "READ_TASK");

    timer_setup(&waking_timer, waking_callback, 0);
    mod_timer(&waking_timer, jiffies + HZ);     // wakes in 1 sec

    return 0;
}


static void mod_exit(void){
    del_timer_sync(&waking_timer); // apparently this one is safer since it waits for a sync'd termination, ask professor later

    kthread_stop(write_task);
    kthread_stop(read_task);
}


module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PBS");
MODULE_DESCRIPTION("Exercise 6B, waiting, writing and reading thread cycle");