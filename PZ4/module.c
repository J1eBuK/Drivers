#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // register_chrdev
#include <linux/uaccess.h>   // copy_to_user, copy_from_user
#include <linux/device.h>    // class_create, device_create
#include <linux/mutex.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/kdev_t.h>

#define DEVICE_NAME "module"      // /dev/module
#define KBUF_SIZE 20

#define module_MAGIC 'p'
#define module_IOC_CLEAR  _IO(module_MAGIC, 0)         /* void */
#define module_IOC_LEN    _IOR(module_MAGIC, 1, int)   /* get int len */

static int majorNumber;
static struct class*  moduleClass  = NULL;
static struct device* moduleDevice = NULL;

static char kbuf[KBUF_SIZE];
static size_t kbuf_len = 0;  /* number of bytes stored, 0..KBUF_SIZE */
static DEFINE_MUTEX(kbuf_mutex);

static void kbuf_consume(size_t n)
{
    if (n >= kbuf_len) {
        kbuf_len = 0;
        return;
    }
    memmove(kbuf, kbuf + n, kbuf_len - n);
    kbuf_len -= n;
}

static ssize_t dev_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset)
{
    ssize_t to_copy;

    if (mutex_lock_interruptible(&kbuf_mutex))
        return -ERESTARTSYS;

    if (kbuf_len == 0) {
        mutex_unlock(&kbuf_mutex);
        return 0; /* EOF / no data */
    }

    to_copy = (len < kbuf_len) ? len : kbuf_len;

    if (copy_to_user(buffer, kbuf, to_copy)) {
        mutex_unlock(&kbuf_mutex);
        return -EFAULT;
    }

    kbuf_consume(to_copy);
    mutex_unlock(&kbuf_mutex);
    return to_copy;
}

static ssize_t dev_write(struct file *filp, const char __user *buffer, size_t len, loff_t *offset)
{
    size_t free_space;
    size_t to_copy;

    if (len == 0)
        return 0;

    if (mutex_lock_interruptible(&kbuf_mutex))
        return -ERESTARTSYS;

    free_space = KBUF_SIZE - kbuf_len;
    if (free_space == 0) {
        mutex_unlock(&kbuf_mutex);
        return -ENOSPC;
    }

    to_copy = (len < free_space) ? len : free_space;

    if (copy_from_user(kbuf + kbuf_len, buffer, to_copy)) {
        mutex_unlock(&kbuf_mutex);
        return -EFAULT;
    }
    kbuf_len += to_copy;
    mutex_unlock(&kbuf_mutex);
    return to_copy;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int len, tmp;

    switch (cmd) {
    case module_IOC_CLEAR:
        if (mutex_lock_interruptible(&kbuf_mutex))
            return -ERESTARTSYS;
        kbuf_len = 0;
        kbuf[0] = '\0';
        mutex_unlock(&kbuf_mutex);
        printk(KERN_INFO "module: buffer cleared via ioctl\n");
        return 0;

    case module_IOC_LEN:
        if (mutex_lock_interruptible(&kbuf_mutex))
            return -ERESTARTSYS;
        len = (int)kbuf_len;
        mutex_unlock(&kbuf_mutex);
        if (copy_to_user((int __user *)arg, &len, sizeof(int)))
            return -EFAULT;
        return 0;

    default:
        return -ENOTTY;
    }
}

static int dev_open(struct inode *inodep, struct file *filep) { return 0; }
static int dev_release(struct inode *inodep, struct file *filep) { return 0; }

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

static int __init module_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        pr_alert("module: failed to register a major number\n");
        return majorNumber;
    }
    pr_info("module: registered with major %d\n", majorNumber);

    moduleClass = class_create(DEVICE_NAME);

    if (IS_ERR(moduleClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        pr_alert("module: failed to create class\n");
        return PTR_ERR(moduleClass);
    }

    moduleDevice = device_create(moduleClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(moduleDevice)) {
        class_destroy(moduleClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        pr_alert("module: failed to create device\n");
        return PTR_ERR(moduleDevice);
    }

    mutex_init(&kbuf_mutex);
    pr_info("module: device /dev/%s created\n", DEVICE_NAME);
    return 0;
}

static void __exit module_exit(void)
{
    device_destroy(moduleClass, MKDEV(majorNumber, 0));
    class_destroy(moduleClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    mutex_destroy(&kbuf_mutex);
    pr_info("module: unregistered\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("PZ4: char device with 20-byte buffer, read/write and ioctls");
module_init(module_init);
module_exit(module_exit);
