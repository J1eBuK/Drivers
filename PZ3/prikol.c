#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "prikoldev"
#define KBUF_SIZE 256

/* ioctl */
#define prikol_MAGIC 'k'
#define prikol_IOC_CLEAR _IO(prikol_MAGIC, 0)
#define prikol_IOC_IS_EMPTY _IOR(prikol_MAGIC, 1, int) //1 - пусто

static int majorNumber;
static struct class*  prikolClass  = NULL;
static struct device* prikolDevice = NULL;

static char kbuf[KBUF_SIZE];
static size_t kbuf_len = 0;
static DEFINE_MUTEX(kbuf_mutex);

static ssize_t dev_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset)
{
    size_t to_copy;

    if (mutex_lock_interruptible(&kbuf_mutex))
        return -ERESTARTSYS;

    if (*offset >= kbuf_len) {
        mutex_unlock(&kbuf_mutex);
        return 0; /* EOF */
    }

    to_copy = kbuf_len - *offset;
    if (len < to_copy)
        to_copy = len;

    if (copy_to_user(buffer, kbuf + *offset, to_copy)) {
        mutex_unlock(&kbuf_mutex);
        return -EFAULT;
    }

    *offset += to_copy;
    mutex_unlock(&kbuf_mutex);
    return to_copy;
}

static ssize_t dev_write(struct file *filp, const char __user *buffer, size_t len, loff_t *offset)
{
    size_t to_copy;

    if (len == 0)
        return 0;

    if (len > KBUF_SIZE - 1)
        to_copy = KBUF_SIZE - 1;
    else
        to_copy = len;

    if (mutex_lock_interruptible(&kbuf_mutex))
        return -ERESTARTSYS;

    if (copy_from_user(kbuf, buffer, to_copy)) {
        mutex_unlock(&kbuf_mutex);
        return -EFAULT;
    }
    kbuf[to_copy] = '\0';
    kbuf_len = to_copy;

    *offset = 0;

    mutex_unlock(&kbuf_mutex);
    return to_copy;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int empty;
    int retval = 0;

    switch (cmd) {
    case prikol_IOC_CLEAR:
        if (mutex_lock_interruptible(&kbuf_mutex))
            return -ERESTARTSYS;
        kbuf_len = 0;
        kbuf[0] = '\0';
        mutex_unlock(&kbuf_mutex);
        printk(KERN_INFO "prikol: buffer cleared via ioctl\n");
        break;

    case prikol_IOC_IS_EMPTY:
        if (mutex_lock_interruptible(&kbuf_mutex))
            return -ERESTARTSYS;
        empty = (kbuf_len == 0) ? 1 : 0;
        mutex_unlock(&kbuf_mutex);
        if (copy_to_user((int __user *)arg, &empty, sizeof(int)))
            return -EFAULT;
        break;

    default:
        retval = -ENOTTY;
        break;
    }

    return retval;
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .open  = dev_open,
    .release = dev_release,
};

static int __init prikol_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "prikol: failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "prikol: registered with major number %d\n", majorNumber);

    prikolClass = class_create(DEVICE_NAME);

    if (IS_ERR(prikolClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "prikol: failed to create device class\n");
        return PTR_ERR(prikolClass);
    }

    prikolDevice = device_create(prikolClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(prikolDevice)) {
        class_destroy(prikolClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "prikol: failed to create the device\n");
        return PTR_ERR(prikolDevice);
    }

    mutex_init(&kbuf_mutex);
    printk(KERN_INFO "prikol: device created /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit prikol_exit(void)
{
    device_destroy(prikolClass, MKDEV(majorNumber, 0));
    class_destroy(prikolClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    mutex_destroy(&kbuf_mutex);
    printk(KERN_INFO "prikol: unregistered and cleaned up\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovkij");
MODULE_DESCRIPTION("Kernel prikol driver");
module_init(prikol_init);
module_exit(prikol_exit);
