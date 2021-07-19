#include <linux/errno.h>
#include <linux/gnss.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <linux/serdev.h>

static struct serdev_device_ops slave_ops ;
int rcv_buf( struct serdev_device * serdev , const unsigned char * buf, size_t n)
{
	char pr_buff[128] = {0};
	int i = 0;
	for(i = 0; i < n; i++)
		pr_buff[i] = buf[i];
	pr_buff[i+1] = 0;
	pr_info("rcv_buff called.%s\n", pr_buff);
	return 0;

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
