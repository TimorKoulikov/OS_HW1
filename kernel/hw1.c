/*
 * kernel/hw1.c  --  HW1 system calls: "The Claude Request Server"
 *
 * Implement the system calls below and submit this file together with every
 * other kernel file you changed (see the assignment PDF). Each kernel service
 * function is named sys_<name> and declared "asmlinkage long".
 */

#include <linux/kernel.h>
#include <linux/sched.h>     /* current, struct task_struct, real_parent */
#include <linux/errno.h>     /* EINVAL */
#include <linux/types.h>     /* pid_t */

/* 333: hello() -- warm-up from the tutorial. Leave it here. */
asmlinkage long sys_hello(void)
{
        printk("Hello, World!\n");
        return 0;
}

/* 334: set_is_paid(is_paid) -- set the current process's paid flag.
 * is_paid is 1 (paid) or 0 (free); return 0, or -EINVAL otherwise. */
asmlinkage long sys_set_is_paid(int is_paid)
{
        printk("[DEBUG]Setting is_paid to %d for pid=%d\n", is_paid, task_tgid_vnr(current));
        if(is_paid != 0 && is_paid != 1)
                return -EINVAL;
        
        current->is_paid = is_paid;
        return 0;
}

/* 335: get_is_paid() -- return the current process's paid flag (0 or 1). */
asmlinkage long sys_get_is_paid(void)
{
        printk("[DEBUG]Getting is_paid for pid=%d: %d\n", task_tgid_vnr(current), current->is_paid);
        return current->is_paid;
}

/* 336: get_ai_token_priority() -- return (pid + parent_pid) % 100,
 * using the user-visible process id (tgid). */
asmlinkage long sys_get_ai_token_priority(void)
{       
        printk("[DEBUG]Calculating priority for pid=%d\n", task_tgid_vnr(current));
        pid_t pid = task_tgid_vnr(current);
        
        // task_ppid_nr() safely handles the RCU lock and dereference for you
        pid_t parent_pid = task_ppid_nr(current); 

        return (pid + parent_pid) % 100;
}
