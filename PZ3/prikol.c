#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "prikoldev"

static int majorNumber;
static const char msg[] = "Tyt 6.bI.Jl Makc  :p\n";

static ssize_t dev_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset)
{
    size_t msg_len = sizeof(msg) - 1;
    size_t to_copy;

    if (*offset >= msg_len)
        return 0;

    to_copy = msg_len - *offset;
    if (len < to_copy)
        to_copy = len;

    if (copy_to_user(buffer, msg + *offset, to_copy))
        return -EFAULT;

    *offset += to_copy;
    return to_copy;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = dev_read,
};

static int __init prikol_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "prikol: failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "prikol: registered correctly with major number %d\n", majorNumber);
    printk(KERN_INFO "prikol: create device node with: sudo mknod /dev/%s c %d 0\n", DEVICE_NAME, majorNumber);
    return 0;
}

static void __exit prikol_exit(void)
{
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "prikol: unregistered module\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("Simple driver for PZ");
module_init(prikol_init);
module_exit(prikol_exit);