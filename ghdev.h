
#ifndef _GH_DEV_H_
#define _GH_DEV_H_

#include <linux/fs.h>
#include <linux/cdev.h>
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
#include <linux/string.h>
struct gh_dev
{
	struct cdev cdev;
	dev_t devno;  // Major and Minor device numbers combined into 32 bits
	struct class *gh_class;  // class_create will set this
	int dev_ready;
	
};

#endif
