#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "prikol"
#define CLASS_NAME  "prikol_class"
#define BUF_SIZE 20

/* ioctl команды */
#define PRIKOL_MAGIC 'p'
#define IOCTL_CLEAR_BUFFER _IO(PRIKOL_MAGIC, 1)
#define IOCTL_IS_EMPTY _IOR(PRIKOL_MAGIC, 2, int)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("Simple driver");
MODULE_VERSION("1.0");

/* ===== Глобальные переменные ===== */

static int major;
static struct class *prikolClass = NULL;
static struct device *prikolDevice = NULL;

static char buffer[BUF_SIZE];
static int buffer_len = 0;

/* ===== Функции работы с устройством ===== */

static int prikol_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "prikol: device opened\n");
    return 0;
}

static int prikol_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "prikol: device closed\n");
    return 0;
}

static ssize_t prikol_write(struct file *file, const char __user *user_buf,
                            size_t len, loff_t *offset)
{
    size_t to_copy;

    to_copy = min(len, (size_t)BUF_SIZE);

    if (copy_from_user(buffer, user_buf, to_copy))
        return -EFAULT;

    buffer_len = to_copy;

    printk(KERN_INFO "prikol: wrote %d bytes\n", buffer_len);
    return buffer_len;
}

static ssize_t prikol_read(struct file *file, char __user *user_buf,
                           size_t len, loff_t *offset)
{
    if (*offset >= buffer_len)
        return 0;

    if (copy_to_user(user_buf, buffer, buffer_len))
        return -EFAULT;

    *offset += buffer_len;

    printk(KERN_INFO "prikol: read %d bytes\n", buffer_len);
    return buffer_len;
}

static long prikol_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int empty;

    switch (cmd) {
    case IOCTL_CLEAR_BUFFER:
        buffer_len = 0;
        memset(buffer, 0, BUF_SIZE);
        printk(KERN_INFO "prikol: buffer cleared\n");
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

/* ===== file_operations ===== */

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = prikol_open,
    .release = prikol_release,
    .read = prikol_read,
    .write = prikol_write,
    .unlocked_ioctl = prikol_ioctl,
};

static int __init prikol_init(void)
{
    printk(KERN_INFO "prikol: initializing module\n");

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "prikol: failed to register major\n");
        return major;
    }

    prikolClass = class_create(CLASS_NAME);
    if (IS_ERR(prikolClass)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(prikolClass);
    }

    prikolDevice = device_create(prikolClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(prikolDevice)) {
        class_destroy(prikolClass);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(prikolDevice);
    }

    printk(KERN_INFO "prikol: device created /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit prikol_exit(void)
{
    device_destroy(prikolClass, MKDEV(major, 0));
    class_destroy(prikolClass);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "prikol: module unloaded\n");
}

module_init(prikol_init);
module_exit(prikol_exit);
