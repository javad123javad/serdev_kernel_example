#ifndef _GH_DEV_H_
#define _GH_DEV_H_

#include <linux/fs.h>
#include <linux/cdev.h>

struct gh_dev
{
	struct cdev cdev;
	dev_t devno;  // Major and Minor device numbers combined into 32 bits
	struct class *gh_class;  // class_create will set this
	int dev_ready;
	
};

#endif
