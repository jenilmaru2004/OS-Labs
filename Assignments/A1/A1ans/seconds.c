/* Information regarding our Linux distribution:

   Distributor ID:	Ubuntu
   Description:	Ubuntu 24.04.1 LTS
   Release:	24.04
   Codename:	noble

   Running the command 'uname -r' gives: 6.8.0-44-generic */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

//we initialize the variable 'begin_jiffies' here which we will use later to calculate the seconds.
unsigned long begin_jiffies;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

/* File operations struct */
static const struct proc_ops my_proc_ops = {
    .proc_read = proc_read,
};

/* This function is called when the module is loaded. */
static int proc_init(void) {
    // Record the time the module is loaded into the kernel. 
    begin_jiffies = jiffies;

    // Create /proc/seconds entry
    // the following function call is a wrapper for
    // proc_create_data() passing NULL as the last argument
    proc_create(PROC_NAME, 0, NULL, &my_proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) {
    // Remove /proc/seconds entry
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

/* We need to calculate how many seconds have passed since the module was loaded.
   We will use the value of jiffies which is a kernel variable that counts the number of 
   ticks since the system has started.
   The HZ rate represents how many jiffies occur in one second. 
   The number of seconds since the module was loaded can be calculated as:
   
   seconds = (ended_jiffies - begin_jiffies)/HZ  */

    unsigned long ended_jiffies = jiffies - begin_jiffies;
    unsigned long elapsed_seconds = ended_jiffies / HZ;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    //%lu here is for printing the type 'unsigned long'. 
    rv = sprintf(buffer, "%lu\n", elapsed_seconds);

    // copies the contents of buffer to userspace usr_buf
    copy_to_user(usr_buf, buffer, rv);

    return rv;
}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Jenil Maru");
