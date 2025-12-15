#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "pz4"
#define CLASS_NAME  "pz4_class"
#define BUF_SIZE    20

/* ioctl команды */
#define PZ4_MAGIC 'p'
#define IOCTL_CLEAR_BUFFER _IO(PZ4_MAGIC, 1)
#define IOCTL_IS_EMPTY     _IOR(PZ4_MAGIC, 2, int)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("PZ4 character driver with buffer and ioctl");
MODULE_VERSION("1.0");

static int major;
static struct class *pz4Class = NULL;
static struct device *pz4Device = NULL;

static char buffer[BUF_SIZE];
static int buffer_len = 0;

static int pz4_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pz4: device opened\n");
    return 0;
}

static int pz4_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pz4: device closed\n");
    return 0;
}

static ssize_t pz4_write(struct file *file, const char __user *user_buf,
                         size_t len, loff_t *offset)
{
    size_t to_copy = min(len, (size_t)BUF_SIZE);

    if (copy_from_user(buffer, user_buf, to_copy))
        return -EFAULT;

    buffer_len = to_copy;

    printk(KERN_INFO "pz4: wrote %d bytes\n", buffer_len);
    return buffer_len;
}

static ssize_t pz4_read(struct file *file, char __user *user_buf,
                        size_t len, loff_t *offset)
{
    if (*offset >= buffer_len)
        return 0;

    if (copy_to_user(user_buf, buffer, buffer_len))
        return -EFAULT;

    *offset += buffer_len;

    printk(KERN_INFO "pz4: read %d bytes\n", buffer_len);
    return buffer_len;
}

static long pz4_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int empty;

    switch (cmd) {
    case IOCTL_CLEAR_BUFFER:
        buffer_len = 0;
        memset(buffer, 0, BUF_SIZE);
        printk(KERN_INFO "pz4: buffer cleared\n");
        break;

    case IOCTL_IS_EMPTY:
        empty = (buffer_len == 0);
        if (copy_to_user((int __user *)arg, &empty, sizeof(int)))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = pz4_open,
    .release = pz4_release,
    .read = pz4_read,
    .write = pz4_write,
    .unlocked_ioctl = pz4_ioctl,
};

/* ===== INIT / EXIT ===== */

static int __init pz4_init(void)
{
    printk(KERN_INFO "pz4: initializing module\n");

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0)
        return major;

    pz4Class = class_create(CLASS_NAME);
    if (IS_ERR(pz4Class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(pz4Class);
    }

    pz4Device = device_create(pz4Class, NULL,
                              MKDEV(major, 0),
                              NULL, DEVICE_NAME);
    if (IS_ERR(pz4Device)) {
        class_destroy(pz4Class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(pz4Device);
    }

    printk(KERN_INFO "pz4: device /dev/%s created\n", DEVICE_NAME);
    return 0;
}

static void __exit pz4_exit(void)
{
    device_destroy(pz4Class, MKDEV(major, 0));
    class_destroy(pz4Class);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "pz4: module unloaded\n");
}

module_init(pz4_init);
module_exit(pz4_exit);
