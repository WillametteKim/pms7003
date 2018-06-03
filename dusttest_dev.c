//ledtest_dev.c
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>

#define GPIO1 16
#define DEV_NAME "dustSensor"
#define DEV_NUM 240

#define RX 16 
#define TX 15
#define Preamble1 0x42
#define Preamble2 0x4d


MODULE_LICENSE("GPL");

int dustSensor_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "OPEN ledtest_dev\n");
	gpio_request(GPIO1, "GPIO1");
	gpio_direction_output(GPIO1, 1);
	return 0;
}

int dustSensor_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "RELEASE ledtest_dev\n");
	return 0;
}