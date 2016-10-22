#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

int init_module(void);
void cleanup_module(void);

static ssize_t proc_read(struct file *, char *, size_t, loff_t *);

static ssize_t device_first_write(struct file *, const char *, size_t, loff_t *);
static ssize_t device_second_write(struct file *, const char *, size_t, loff_t *);
static ssize_t device_operand_write(struct file *, const char *, size_t, loff_t *);


#define DEVICE_OPERAND "operand"
#define DEVICE_FIRST "first"
#define DEVICE_SECOND "second"
#define BUF_LEN 80              /* Max length of the message from the device */


static int Major1;              /* Major operand  assigned to our device driver */
static int Major2;              /* Major first assigned to our device driver */
static int Major3;              /* Major second assigned to our device driver */

static char result[BUF_LEN];
static char first[BUF_LEN];
static char second[BUF_LEN];
static char operand[BUF_LEN];


static struct file_operations proc_fops = 
{
        .read = proc_read,
};

static struct file_operations device_first_fops = 
{
        .write = device_first_write,
};

static struct file_operations device_second_fops = 
{
        .write = device_second_write,
};

static struct file_operations device_operand_fops = 
{
        .write = device_operand_write,
};

static struct proc_dir_entry *proc_entry_result;


int init_module(void)
{
    memset(result, '0', 3);
    proc_entry_result = proc_create( "result", 0, NULL, &proc_fops);

    Major1 = register_chrdev(0, DEVICE_OPERAND, &device_operand_fops);
    Major2 = register_chrdev(0, DEVICE_FIRST, &device_first_fops);
    Major3 = register_chrdev(0, DEVICE_SECOND, &device_second_fops);

        if (Major1 < 0) 
	{
          printk(KERN_ALERT "Registering char device failed with %d\n", Major1);
          return Major1;
        }

        if (Major2 < 0) 
	{
          printk(KERN_ALERT "Registering char device failed with %d\n", Major2);
          return Major2;
        }


        if (Major3 < 0) 
	{
          printk(KERN_ALERT "Registering char device failed with %d\n", Major3);
          return Major3;
        }


        printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_OPERAND, Major1);
        printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_FIRST, Major2);
        printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_SECOND, Major3);

        return 0;
}


void cleanup_module(void)
{

    unregister_chrdev(Major1, DEVICE_OPERAND);
    unregister_chrdev(Major2, DEVICE_FIRST);
    unregister_chrdev(Major3, DEVICE_SECOND);

    remove_proc_entry("result", NULL);
}


static ssize_t proc_read(struct file *filp,     /* see include/linux/fs.h   */
                           char *buffer,        /* buffer to fill with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{
    ssize_t cnt = strlen(result), ret;
    int nfirst;
    int nsecond;
    int nresult;
    sscanf(first, "%d", &nfirst);
    sscanf(second, "%d", &nsecond);

    if(operand[0] == 'm') 
    {
        nresult = nfirst * nsecond;
        sprintf(result, "%3d", nresult);
    }
    else if(operand[0] == '/') 
    {
        nresult = nfirst / nsecond;
        sprintf(result, "%3d", nresult);
    }
    else if(operand[0] == '-') 
    {
        nresult = nfirst - nsecond;
        sprintf(result, "%3d", nresult);
    }
    else if(operand[0] == '+') 
    {
        nresult = nfirst + nsecond;
        sprintf(result, "%3d", nresult);
    }

    ret = copy_to_user(buffer, result, cnt);
    *offset += cnt - ret;

    if (*offset > cnt)
        return 0;
    else
        return cnt;
}

static ssize_t device_first_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    if ( copy_from_user(first, buff, len) ) 
    {
        return -EFAULT;
    }
    return len;
}

static ssize_t device_second_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    if ( copy_from_user(second, buff, len) ) 
    {
        return -EFAULT;
    }
    return len;
}

static ssize_t device_operand_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    if ( copy_from_user(operand, buff, len) ) 
    {
        return -EFAULT;
    }
    return len;
}

