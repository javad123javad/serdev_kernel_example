#include "ghdev.h"

static struct serdev_device_ops slave_ops ;
static struct gh_dev *gdev;

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: read()\n");
    return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len,
    loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    return len;
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};


static int gh_chrdev_register(void)
{
	int ret;
        struct device *dev_ret;

	gdev = kzalloc(sizeof(struct gh_dev), GFP_KERNEL);
	if(NULL == gdev)
	{
		pr_err("Unable to allocate kernel mem.\n");
		return -1;
	}
	 
        printk(KERN_INFO "Allocating new region to cdev");
	if ((ret = alloc_chrdev_region(&gdev->devno, 0, 1, "ghregion")) < 0)
    	{
        	return ret;
    	}
    	
	if (IS_ERR(gdev->gh_class = class_create(THIS_MODULE, "ghclass")))
    	{
		pr_info("Unable to create class.");
        	unregister_chrdev_region(gdev->devno, 1);
        	return PTR_ERR(gdev->gh_class);
    	}

	    
	if (IS_ERR(dev_ret = device_create(gdev->gh_class, NULL, gdev->devno, NULL, "ghcntrl")))
    	{
        	class_destroy(gdev->gh_class);
        	unregister_chrdev_region(gdev->devno, 1);
        	return PTR_ERR(dev_ret);
    	}
	

    	cdev_init(&gdev->cdev, &pugs_fops);
    	if ((ret = cdev_add(&gdev->cdev, gdev->devno, 1) < 0))
    	{
        	device_destroy(gdev->gh_class, gdev->devno);
        	class_destroy(gdev->gh_class);
        	unregister_chrdev_region(gdev->devno, 1);
        	return ret;
        }
	pr_info("Device Created sucessfully.\n");
	gdev->dev_ready = 1;
    return 0;
}

static void gh_chrdev_unregister(void)
{
	if(1 == gdev->dev_ready)
	{
    		device_destroy(gdev->gh_class, gdev->devno);
    		class_destroy(gdev->gh_class);
                cdev_del(&gdev->cdev);
		unregister_chrdev_region(gdev->devno, 1);
		gdev->dev_ready = 0;
		kfree(gdev);
		pr_info("Chardev removed.\n");
	}
	
}
/*************** msg parser *******************/
static int msg_parser(const u8 buf[], size_t len)
{
	int ret = 0;

	return 0;
	
}


/**** Serdev Methods ******/
int rcv_buf( struct serdev_device * serdev , const unsigned char * buf, size_t n)
{
	char pr_buff[128] = {0};
	int i = 0;
	for(i = 0; i < n; i++)
		pr_buff[i] = buf[i];
	pr_buff[i+1] = 0;
	pr_info("rcv_buff called.%s\n", pr_buff);
	gh_chrdev_register();
	return n;

}
static int gh_probe(struct serdev_device *serdev) {
  int iret = 0;
  unsigned char msg[] = "hello";
  slave_ops.receive_buf = rcv_buf;
  serdev_device_set_client_ops(serdev, &slave_ops);
  iret = serdev_device_open(serdev);
  if (iret < 0) {
    pr_err("Error Openning Serial Device.\n");
    return iret;
  }

  pr_info("Serial device openned.\n");
  serdev_device_set_baudrate(serdev, 115200);
  serdev_device_write_buf(serdev,msg,5);
  return 0;
}

static void gh_remove(struct serdev_device *serdev) {
  gh_chrdev_unregister();
  serdev_device_close(serdev);
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
