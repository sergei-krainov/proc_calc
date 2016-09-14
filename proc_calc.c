#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


#define NFR 3                   /* Number of /proc files for reading */
#define NFW 1                   /* Number of /proc files for writing */

#define PF1 "operator1"         /* Proc file names */
#define PF2 "operator2"
#define PF3 "operand"
#define PF4 "result"

char *an[4] = {PF1, PF2, PF3, PF4};

struct proc_dir_entry *f1;
struct proc_dir_entry *f2;
struct proc_dir_entry *f3;
struct proc_dir_entry *f4;

struct proc_dir_entry *our_proc_file;

static int hello_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello proc!\n");
    return 0;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
    return single_open(file, hello_proc_show, NULL);
}

static const struct file_operations hello_proc_fops = {
    .owner = THIS_MODULE,
    .open = hello_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init hello_proc_init(void) {
    short int i;
    
    for (i=0; i < 4; i++) {
        our_proc_file = proc_create(an[i], 0, NULL, &hello_proc_fops);
        
        if (our_proc_file == NULL) {
        printk(KERN_INFO "Procfs is null\n");
        }
        else {
            printk(KERN_INFO "Procfs is not null\n");
        }
    }
    
    //our_proc_file = proc_create(PROCFS_NAME, 0, NULL, &hello_proc_fops);
    return 0;
}

static void __exit hello_proc_exit(void) {
    short int i;
    
    for (i=0; i < 4; i++) {
        remove_proc_entry(an[i], NULL);
    }
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
