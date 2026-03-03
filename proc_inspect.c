// proc_inspect.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h>
#include <linux/pid.h>
#include <linux/capability.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define PROC_NAME "proc_inspect"

static struct proc_dir_entry *proc_entry;

static int inspect_all = 1;
static int target_pid = -1;

/* Print process information */
static void inspect_process_seq(struct seq_file *m, struct task_struct *task)
{
    seq_printf(m,
        "PID: %d | Name: %-16s | State: %c | PPID: %d\n",
        task->pid,
        task->comm,
        task_state_to_char(task),
        task->real_parent->pid
    );
}

/* PROCFS show handler */
static int proc_show(struct seq_file *m, void *v)
{
    struct task_struct *task;

    if (!capable(CAP_SYS_ADMIN)) {
        seq_printf(m, "Permission denied – requires CAP_SYS_ADMIN\n");
        return 0;
    }

    if (inspect_all) {
        for_each_process(task) {
            inspect_process_seq(m, task);
        }
    } else {
        struct pid *pid_struct = find_get_pid(target_pid);

        if (!pid_struct) {
            seq_printf(m, "Invalid PID: %d\n", target_pid);
            return 0;
        }

        task = pid_task(pid_struct, PIDTYPE_PID);

        if (!task) {
            seq_printf(m, "No task found for PID %d\n", target_pid);
            return 0;
        }

        inspect_process_seq(m, task);
    }

    return 0;
}

/* Open handler */
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

/* Write handler: set PID or "all" */
static ssize_t proc_write(struct file *file,
                          const char __user *buffer,
                          size_t count,
                          loff_t *pos)
{
    char kbuf[32];

    if (count >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buffer, count))
        return -EFAULT;

    kbuf[count] = '\0';

    if (strncmp(kbuf, "all", 3) == 0) {
        inspect_all = 1;
    } else {
        if (kstrtoint(kbuf, 10, &target_pid) == 0) {
            inspect_all = 0;
        } else {
            return -EINVAL;
        }
    }

    return count;
}

/* File operations (modern kernels use proc_ops) */
static const struct proc_ops proc_file_ops = {
    .proc_open    = proc_open,
    .proc_read    = seq_read,
    .proc_write   = proc_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* Module init */
static int __init proc_init(void)
{
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);

    if (!proc_entry) {
        pr_err("Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created successfully\n", PROC_NAME);
    return 0;
}

/* Module exit */
static void __exit proc_exit(void)
{
    proc_remove(proc_entry);
    pr_info("/proc/%s removed\n", PROC_NAME);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Process Inspector via PROCFS (Kernel 5.15 compatible)");
MODULE_VERSION("1.0");

module_init(proc_init);
module_exit(proc_exit);
