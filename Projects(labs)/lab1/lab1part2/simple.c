#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>  // For init_task

/* Function to print the PCB of init_task (pid 0) */
void print_init_PCB(void) {
    struct task_struct *task = &init_task;

    printk(KERN_INFO "init_task pid: %d\n", task->pid);
    printk(KERN_INFO "init_task state: %ld\n", task->__state); // Printing task state
    printk(KERN_INFO "init_task flags: %lu\n", task->flags);  // Printing flags
    printk(KERN_INFO "init_task runtime priority: %d\n", task->rt_priority); // Printing rt_priority
    printk(KERN_INFO "init_task process policy: %u\n", task->policy); // Printing policy
    printk(KERN_INFO "init_task task group id (tgid): %d\n", task->tgid); // Printing tgid
}

/* This function is called when the module is loaded */
int simple_init(void)
{
    printk(KERN_INFO "Loading Module\n");

    // Call function to print init_task PCB
    print_init_PCB();

    return 0;
}

/* This function is called when the module is removed */
void simple_exit(void) {
    printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module to Print init_task PCB");
MODULE_AUTHOR("SGG");

