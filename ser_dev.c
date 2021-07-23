#include <linux/errno.h>
#include <linux/gnss.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <linux/serdev.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include "ghdev.h"

static struct serdev_device_ops slave_ops ;
static struct gh_dev *gdev;

/* Char Device methods */
//static int gh_chdev_register(const char name_to_reg[], struct module* owner);

static int ghch_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}
static int ghch_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}
static ssize_t ghch_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: read()\n");
    return 0;
}
static ssize_t ghch_write(struct file *f, const char __user *buf, size_t len,
    loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    return len;
}

static struct file_operations gh_fops =
{
    .owner = THIS_MODULE,
    .open = ghch_open,
    .release = ghch_close,
    .read = ghch_read,
    .write = ghch_write
};



static int gh_chdev_register(void)
{
	int ret;
	struct device *dev_ret;
	gdev = kzalloc(sizeof(struct gh_dev), GFP_KERNEL);

	ret = alloc_chrdev_region(&gdev->devno, 0, 1, "ghregion");
	if(ret < 0)
	{
		pr_err("Unable to allocate region\n");
		return ret;
	}
	
	gdev->gh_class = class_create(THIS_MODULE, "gh_class");
	if(IS_ERR(gdev->gh_class))
	{
		pr_err("Unable to create class\n");
		class_destroy(gdev->gh_class);
		unregister_chrdev_region(gdev->devno,1);
		return PTR_ERR(gdev->gh_class);
	}
	
	dev_ret = device_create(gdev->gh_class, NULL, gdev->devno, NULL, "ghcntrl");
	if(IS_ERR(dev_ret))
	{
		pr_err("Unable to create class\n");
                class_destroy(gdev->gh_class);
	        unregister_chrdev_region(gdev->devno,1);
		return PTR_ERR(dev_ret);
	}
	cdev_init(&gdev->cdev, &gh_fops);
	
	ret = cdev_add(&gdev->cdev, gdev->devno,1);
	if(ret < 0)
	{
		pr_err("Unable to add cdev\n");
		device_destroy(gdev->gh_class, gdev->devno);
		class_destroy(gdev->gh_class);
                unregister_chrdev_region(gdev->devno,1);
		return ret;

	}
	pr_info("Device created and added successfully\n");
	gdev->dev_ready = 1;
	return 0;
}

static int gh_chdev_unregister(void)
{
	if(gdev->dev_ready == 1)
	{
	 cdev_del(&gdev->cdev);
	 device_destroy(gdev->gh_class, gdev->devno);
	 class_destroy(gdev->gh_class);
	 unregister_chrdev_region(gdev->devno,1);
	 kfree(gdev);
	}
         
	return 0;
}
/*****************************************/


static void parse_recv_msg(const char buf[], const size_t len)
{
	u8 tmp_buf[32 + 1] = {0};
	strncpy(tmp_buf, buf, len);
	if(0 == strncmp(tmp_buf,"ATGH",4))
	{
		pr_info("Device Match. registering char Dev\n");
		gh_chdev_register();
		
	}
}
int rcv_buf( struct serdev_device * serdev , const unsigned char * buf, size_t n)
{	
		
	pr_info("rcv_buff called.%s\n", buf);
	parse_recv_msg(buf, n);
	return n;

}

static int gh_probe(struct serdev_device *serdev) {
  int iret = 0;
  unsigned char msg[] = "AT\r\n";
  slave_ops.receive_buf = rcv_buf;
  serdev_device_set_client_ops(serdev, &slave_ops);
  iret = serdev_device_open(serdev);
  if (iret < 0) {
    pr_err("Error Openning Serial Device.\n");
    return iret;
  }

  pr_info("Serial device openned.\n");
  serdev_device_set_baudrate(serdev, 115200);
  serdev_device_write_buf(serdev,msg,4);
  return 0;
}

static void gh_remove(struct serdev_device *serdev) {
	gh_chdev_unregister();
  serdev_device_close(serdev);
  //greenhouse_dev_unregister();
  pr_info("Serial Device Closed.\n");
}

static const struct of_device_id greenhouse_of_match[] = {
    {.compatible = "jrp,greenhouse"},
    {},
};
MODULE_DEVICE_TABLE(of, greenhouse_of_match);
static struct serdev_device_driver jrp_geenhouse_driver = {
    .driver =
        {
            .name = "gnss",
            .of_match_table = of_match_ptr(greenhouse_of_match),
            .pm		= NULL,
        },
    .probe = gh_probe,
    .remove = gh_remove,
};
module_serdev_device_driver(jrp_geenhouse_driver);


MODULE_AUTHOR("Javad Rahimi <javad321javad@gmail.com>");
MODULE_DESCRIPTION("My Green house driver");
MODULE_LICENSE("GPL v2");
