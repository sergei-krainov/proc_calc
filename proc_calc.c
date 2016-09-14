#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>


#define NFR 3                   /* Number of /proc files for reading */
#define NFW 1                   /* Number of /proc files for writing */
#define NFA (NFR + NFW)         /* Number of all /proc files to be created */

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

/* Functions for output files starts here */

static int hello_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello proc!\n");
    return 0;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
    return single_open(file, hello_proc_show, NULL);
}

//ssize_t read_not_permitted(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
//	printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
//    printk(KERN_INFO "Read operation is not permitted for this file\n");
//	return 0;
//}

static char test[] = "testing";

ssize_t my_write(struct file* filp, const char __user* buf, size_t len, loff_t* offset)
{
    int pos;
    int size;
    
    pos = *offset;
    size = 32;
    
    if (pos >= size) pos = size;
    if ((pos + len) >= size) len = (size - pos) - 1;
    /* copy count from a user space buffer to the kernel buffer */
    /* copy_from_user returns 0 on success */                    
    if (copy_from_user(test,buf,len))               
       return -EFAULT;  
    pos += len;
    //filp->offset = pos;  // Note Deprecated                    
    *offset = pos; // new for 2.6 Kernel   
    return len;
}

ssize_t my_read(struct file* filp, const char __user* buf, size_t len, loff_t* offset)
{
    
    int pos;
    int size;
    
    pos = *offset;
    size = 32;
    
    if (pos >= size) pos = size - 1;
    if ((pos + len) >= size) len = (size - pos) - 1;
    
     /* copy count from kernel buffer to user space buffer */            
     /* copy_to_user returns 0 on success */                             
     if (copy_to_user(buf,(void*)test,len))                        
        return -EFAULT;                                                  
     pos += len;                                                       
     //filep->f_pos = pos; // Note this is deprecated                            
     *offset = len;  // new for 2.6 Kernel                                
    
    return 0;
}

/* Functions for output files ends here */

/* Functions for input files starts here */

ssize_t read_not_permitted(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
	printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    printk(KERN_INFO "Read operation is not permitted for this file\n");
	return 0;
}

/* Functions for input files ends here */

static const struct file_operations proc_fops_output = {
    .owner = THIS_MODULE,
    .open = hello_proc_open,
    //.read = seq_read,
    .read = my_read,
    .write  = my_write,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations proc_fops_input = {
    .owner = THIS_MODULE,
    .open = hello_proc_open,
    .read = read_not_permitted,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init proc_calc_init(void) {
    short int i;
    
    /* Creating devices for input */
    /* We will write data to them */
    for (i=0; i < NFR; i++) {
        our_proc_file = proc_create(an[i], 0, NULL, &proc_fops_input);
        
        if (our_proc_file == NULL) {
        printk(KERN_INFO "Procfs is null\n");
        }
        else {
            printk(KERN_INFO "Procfs is not null\n");
            printk(KERN_INFO "File /proc/%s created\n", an[i]);
        }
    }
    
    /* Creating devices for input */
    /* We will read data from them */
    for (; i < NFA; i++) {
        our_proc_file = proc_create(an[i], 0, NULL, &proc_fops_output);
        
        if (our_proc_file == NULL) {
        printk(KERN_INFO "Procfs is null\n");
        }
        else {
            printk(KERN_INFO "Procfs is not null\n");
            printk(KERN_INFO "File /proc/%s created\n", an[i]);
        }
    }
    
    return 0;
}

static void __exit proc_calc_exit(void) {
    short int i;
    
    for (i=0; i < NFA; i++) {
        remove_proc_entry(an[i], NULL);
    }
}

MODULE_LICENSE("GPL");
module_init(proc_calc_init);
module_exit(proc_calc_exit);
