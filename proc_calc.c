#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>

MODULE_LICENSE("GPL");


#define NFR 2                   /* Number of /proc files for writing, operators */
#define NFO 1                   /* Number of /proc files for writing, operand */
#define NFW 1                   /* Number of /proc files for reading */
#define NFA (NFR + NFO + NFW)   /* Number of all /proc files to be created */

#define PF1 "operator1"         /* Proc file names */
#define PF2 "operator2"
#define PF3 "operand"
#define PF4 "result"

char *an[4] = {PF1, PF2, PF3, PF4};

int var1 = 0;
int var2 = 0;
int result = 0;
char operand;

char *tmp;

struct proc_dir_entry *our_proc_file;

/* Functions for output files starts here */

static int proc_show_result(struct seq_file *m, void *v) {
    result = 0;
    
    if (operand == 0) {
        seq_printf(m, "Empty or incorrect operand. Processing stopped\n");
        return 0;
    }
    
    switch (operand) {
    case '+':
        result = var1 + var2;
        break;
    case '-':
        result = var1 - var2;
        break;
    case '*':
        result = var1 * var2;
        break;
    case '/':
        if ( var2 == 0 ) {
            seq_printf(m, "You can't devide by zero\n");
        }
        else {
            result = var1 / var2;
        }
        break;
    }
    
    seq_printf(m, "%d\n", result);
    
    return 0;
}

static int open_proc_file(struct inode *inode, struct  file *file) {
    return single_open(file, proc_show_result, NULL);
}

ssize_t write_not_permitted(struct file *filp, const char __user *buffer, size_t length, loff_t *offset) {
    printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    printk(KERN_INFO "Write operation is not permitted for this file\n");
    return length;
}

/* Functions for output files ends here */

/* Functions for input files starts here */

ssize_t my_write_op1(struct file* filp, const char __user* buf, size_t len, loff_t* offset)
{
    printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    if ( len < 0 )
        return -EINVAL;
    
    var1 = simple_strtoll(buf, 0, 0);
    printk(KERN_INFO "var1 = %d\n", var1 );
    
    return len;
}

ssize_t my_write_op2(struct file* filp, const char __user* buf, size_t len, loff_t* offset)
{
    printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    if ( len < 0 )
        return -EINVAL;
    
    var2 = simple_strtoll(buf, 0, 0);    
    printk(KERN_INFO "var2 = %d\n", var2 );
    
    return len;
}

ssize_t my_write_operand(struct file* filp, const char __user* buf, size_t len, loff_t* offset)
{
    int i = 0;
    
    printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    if ( len < 0 )
        return -EINVAL;
     
    while (isspace(buf[i]))
        i++;
    operand = buf[i];
    
    if ( operand != '+' && operand != '-' && operand != '*' && operand != '/' ) {
        operand = 0;
    }
        
    printk(KERN_INFO "operand = %c\n", operand );
    
    return len;
}

ssize_t read_not_permitted(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    printk(KERN_INFO "Entering function %s\n", __FUNCTION__ );
    printk(KERN_INFO "Read operation is not permitted for this file\n");
    return 0;
}

/* Functions for input files ends here */

static const struct file_operations proc_fops_output = {
    .owner = THIS_MODULE,
    .open = open_proc_file,
    .read = seq_read,
    .write = write_not_permitted,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations proc_fops_input_op1 = {
    .owner = THIS_MODULE,
    .open = open_proc_file,
    .read = read_not_permitted,
    .write  = my_write_op1,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations proc_fops_input_op2 = {
    .owner = THIS_MODULE,
    .open = open_proc_file,
    .read = read_not_permitted,
    .write  = my_write_op2,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations proc_fops_input_operand = {
    .owner = THIS_MODULE,
    .open = open_proc_file,
    .read = read_not_permitted,
    .write  = my_write_operand,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init proc_calc_init(void) {    
    short int i;
    
    printk(KERN_INFO "Module proc_calc loaded\n");
    
    /* Creating devices for input */
    /* We will write data to them */
    /* Two files for operators */
    for (i=0; i < NFR; i++) {
        our_proc_file = proc_create(an[i], 0, NULL, (i == 0) ? &proc_fops_input_op1
                                                             : &proc_fops_input_op2 );
        
        if (our_proc_file == NULL) {
        printk(KERN_INFO "Procfs is null\n");
        }
        else {
            printk(KERN_INFO "Procfs is not null\n");
            printk(KERN_INFO "File /proc/%s created\n", an[i]);
        }
    }
    
    /* One file for operand */
    our_proc_file = proc_create(an[i], 0, NULL, &proc_fops_input_operand);
    
    if (our_proc_file == NULL) {
    printk(KERN_INFO "Procfs is null\n");
    }
    else {
        printk(KERN_INFO "Procfs is not null\n");
        printk(KERN_INFO "File /proc/%s created\n", an[i]);
    }
    i++;
    
    /* Creating devices for output */
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
    
    tmp = kmalloc(sizeof(int), GFP_KERNEL);
    
    return 0;
}

static void __exit proc_calc_exit(void) {
    short int i;
    
    for (i=0; i < NFA; i++) {
        remove_proc_entry(an[i], NULL);
        printk(KERN_INFO "File /proc/%s deleted\n", an[i]);
    }
    
    printk(KERN_INFO "Module proc_calc unloaded\n");
}

module_init(proc_calc_init);
module_exit(proc_calc_exit);
