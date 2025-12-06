// prikol_auto.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // register_chrdev / unregister_chrdev
#include <linux/uaccess.h>   // copy_to_user
#include <linux/device.h>    // class_create, device_create, device_destroy, class_destroy
#include <linux/kdev_t.h>    // MKDEV

#define DEVICE_NAME "prikoldev"

static int majorNumber;
static struct class*  prikolClass  = NULL;
static struct device* prikolDevice = NULL;

static const char msg[] = "Tyt 6.bl.Jl Makc!\n";

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

/* Инициализация */
static int __init prikol_init(void)
{
    /* 1) Зарегистрировать символьный драйвер, получить major */
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "prikol: failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "prikol: registered with major number %d\n", majorNumber);

    /* 2) Создать класс в sysfs (для udev/devtmpfs) */
    prikolClass = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(prikolClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "prikol: failed to create device class\n");
        return PTR_ERR(prikolClass);
    }
    printk(KERN_INFO "prikol: device class created\n");

    /* 3) Создать устройство — это приведёт к появлению /dev/prikoldev (udev/devtmpfs) */
    prikolDevice = device_create(prikolClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(prikolDevice)) {
        class_destroy(prikolClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "prikol: failed to create the device\n");
        return PTR_ERR(prikolDevice);
    }

    printk(KERN_INFO "prikol: device created: /dev/%s\n", DEVICE_NAME);
    return 0;
}

/* Выгрузка */
static void __exit prikol_exit(void)
{
    /* Удаляем устройство и класс */
    if (prikolDevice)
        device_destroy(prikolClass, MKDEV(majorNumber, 0));
    if (prikolClass)
        class_destroy(prikolClass);

    /* Отменяем регистрацию major */
    unregister_chrdev(majorNumber, DEVICE_NAME);

    printk(KERN_INFO "prikol: unregistered and cleaned up\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Maximov");
MODULE_DESCRIPTION("Simple char device that auto-creates /dev node");
module_init(prikol_init);
module_exit(prikol_exit);
