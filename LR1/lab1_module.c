#include <linux/string.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "lab1dev"
#define BIN_WIDTH_US 50
#define MAX_BINS 20

#define IOCTL_GET_HIST_LEN _IOR('x', 1, int)
#define IOCTL_GET_HIST_BUF _IOR('x', 2, size_t[MAX_BINS])

static dev_t dev_id;
static struct cdev lab_cdev;
static struct class *lab_class;

static int value_buf;
static bool value_valid;

static size_t bins[MAX_BINS];
static size_t active_bins = 1;

static unsigned long bin_elapsed_us;
static unsigned long prev_jiffies;

static void update_histogram(unsigned long delta_us)
{
    bin_elapsed_us += delta_us;
    bins[active_bins - 1]++;

    if (bin_elapsed_us >= BIN_WIDTH_US) {
        bin_elapsed_us = 0;
        if (active_bins < MAX_BINS)
            active_bins++;
    }
}

static int lab_open(struct inode *inode, struct file *file)
{
    pr_info("lab1: open\n");

    value_valid = false;
    active_bins = 1;
    bin_elapsed_us = 0;
    prev_jiffies = jiffies;

    memset(bins, 0, sizeof(bins));

    return 0;
}

static int lab_release(struct inode *inode, struct file *file)
{
    pr_info("lab1: close\n");
    return 0;
}

static ssize_t lab_write(struct file *file,
                         const char __user *buf,
                         size_t len,
                         loff_t *ppos)
{
    if (len != sizeof(int))
        return -EINVAL;

    if (copy_from_user(&value_buf, buf, sizeof(int)))
        return -EFAULT;

    value_valid = true;
    prev_jiffies = jiffies;

    return sizeof(int);
}

static ssize_t lab_read(struct file *file,
                        char __user *buf,
                        size_t len,
                        loff_t *ppos)
{
    unsigned long now, delta_j, delta_us;

    if (!value_valid)
        return 0;

    if (len != sizeof(int))
        return -EINVAL;

    if (copy_to_user(buf, &value_buf, sizeof(int)))
        return -EFAULT;

    now = jiffies;
    delta_j = now - prev_jiffies;
    delta_us = (delta_j * USEC_PER_SEC) / HZ;

    update_histogram(delta_us);

    prev_jiffies = now;

    return sizeof(int);
}

static long lab_ioctl(struct file *file,
                      unsigned int cmd,
                      unsigned long arg)
{
    switch (cmd) {
    case IOCTL_GET_HIST_LEN:
        if (copy_to_user((size_t __user *)arg,
                         &active_bins,
                         sizeof(active_bins)))
            return -EFAULT;
        break;

    case IOCTL_GET_HIST_BUF:
        if (copy_to_user((size_t __user *)arg,
                         bins,
                         active_bins * sizeof(size_t)))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations lab_fops = {
    .owner = THIS_MODULE,
    .open = lab_open,
    .release = lab_release,
    .read = lab_read,
    .write = lab_write,
    .unlocked_ioctl = lab_ioctl
};

static int __init lab_init(void)
{
    if (alloc_chrdev_region(&dev_id, 0, 1, DEVICE_NAME))
        return -EINVAL;

    cdev_init(&lab_cdev, &lab_fops);
    if (cdev_add(&lab_cdev, dev_id, 1))
        goto err_cdev;

    lab_class = class_create(DEVICE_NAME);
    if (IS_ERR(lab_class))
        goto err_class;

    device_create(lab_class, NULL, dev_id, NULL, DEVICE_NAME);

    pr_info("lab1: module loaded\n");
    return 0;

err_class:
    cdev_del(&lab_cdev);
err_cdev:
    unregister_chrdev_region(dev_id, 1);
    return -EINVAL;
}

static void __exit lab_exit(void)
{
    device_destroy(lab_class, dev_id);
    class_destroy(lab_class);
    cdev_del(&lab_cdev);
    unregister_chrdev_region(dev_id, 1);
    pr_info("lab1: module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("Lab1 char driver");

module_init(lab_init);
module_exit(lab_exit);