#include "asm/uaccess.h"
#include "linux/err.h"
#include "linux/printk.h"
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

#define True 0
#define False -1

static int major;
static struct class *led_class;
static struct gpio_desc *led_gpios;

ssize_t led_read(struct file *file, char __user *buf, size_t size,
                 loff_t *offset) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return True;
}

ssize_t led_write(struct file *file, const char __user *buf, size_t size,
                  loff_t *offset) {
  int err;
  unsigned char status;
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  if (!led_gpios)
    goto out;

  err = copy_from_user(&status, buf, 1);
  gpiod_set_value(led_gpios, status);
  return 1;
out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return false;
}

int led_open(struct inode *node, struct file *file) {
  if (!led_gpios)
    goto out;
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  gpiod_direction_output(led_gpios, 0);
  return True;
out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return False;
}

int led_close(struct inode *node, struct file *file) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return True;
}

/**
 * led file operations
 */
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_close,
};

/**
 * led device probe
 */
int led_probe(struct platform_device *pdev) {
  struct device *class_dev;
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  led_gpios = gpiod_get(&pdev->dev, "led", 0);
  if (IS_ERR(led_gpios))
    goto out;

  major = register_chrdev(0, "led", &led_fops);
  if (major < 0) {
    gpiod_put(led_gpios);
    goto out;
  }

  led_class = class_create(THIS_MODULE, "led");
  if (IS_ERR(led_class)) {
    unregister_chrdev(major, "led");
    gpiod_put(led_gpios);
    goto out;
  }

  class_dev =
      device_create(led_class, NULL, MKDEV(major, 0), NULL, "imx_led%d", 0);
  if (IS_ERR(class_dev)) {
    class_destroy(led_class);
    unregister_chrdev(major, "led");
    gpiod_put(led_gpios);
    goto out;
  }

  return True;
out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return false;
}

/**
 * led device remove
 */
int led_remove(struct platform_device *pdev) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  device_destroy(led_class, MKDEV(major, 0));
  class_destroy(led_class);
  unregister_chrdev(major, "led");

  return True;
}

static const struct of_device_id imx_led_ids[] = {
    {.compatible = "imx,led"},
    {},
};

static struct platform_driver led_drv = {
    .probe = led_probe,
    .remove = led_remove,
    .driver =
        {
            .name = "imx_led",
            .of_match_table = imx_led_ids,
        },
};

/**
 * led driver register
 */
static int __init led_init(void) {
  platform_driver_register(&led_drv);
  return True;
}

/**
 * led driver unregister
 */
static void __exit led_exit(void) { platform_driver_unregister(&led_drv); }

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");