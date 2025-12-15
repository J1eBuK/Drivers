#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/device/class.h>

#define DEVICE_NAME "prikol"

static struct class  *simple_class;
static dev_t dev_num;
static struct cdev simple_cdev;
static struct device *simple_device;

static int dev_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "prikol: device opened\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file *file){
	printk(KERN_INFO "prikol: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
	char msg[] = "Tyt 6.I.JL Makc!\n";
	int bytes = sizeof(msg);

	if(*offset >= bytes){
		return 0;
	}

	if(copy_to_user(buf, msg, bytes))
		return -EFAULT;

	*offset += bytes;
	printk(KERN_INFO "prikol: read %d bytes\n", bytes);
	return bytes;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
};

static int __init simple_init(void)
{
	int res = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (res < 0){
		printk(KERN_ALERT "prikol: alloc_chrdev_region failed\n");
		return res;
	}

	printk(KERN_INFO "prikol: registered <major %d, minor %d>\n", MAJOR(dev_num), MINOR(dev_num));

	cdev_init(&simple_cdev, &fops);
	simple_cdev.owner = THIS_MODULE;
	res = cdev_add(&simple_cdev, dev_num, 1);
	if (res < 0) {
		unregister_chrdev_region(dev_num, 1);
		printk(KERN_ALERT "prikol: cdev_add failed\n");
		return res;
	}

	simple_class = class_create(DEVICE_NAME);
	if (IS_ERR(simple_class)) {
		cdev_del(&simple_cdev);
		unregister_chrdev_region(dev_num, 1);
		printk(KERN_ALERT "prikol: class_create failed\n");
		return PTR_ERR(simple_class);
	}

	simple_device = device_create(simple_class, NULL, dev_num, NULL, DEVICE_NAME);
	if(IS_ERR(simple_device)) {
		class_destroy(simple_class);
		cdev_del(&simple_cdev);
		unregister_chrdev_region(dev_num, 1);
		printk(KERN_ALERT "prikol: device_create failed\n");
		return PTR_ERR(simple_device);
	}

	printk(KERN_INFO "prikol: device created successfully\n");
	return 0;

}

static void __exit simple_exit(void)
{
	device_destroy(simple_class, dev_num);
	class_destroy(simple_class);
	cdev_del(&simple_cdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO "prikol: unregistered and cleaned up\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Stefanovskij");
MODULE_DESCRIPTION("Message from MAKC0H4NK");
