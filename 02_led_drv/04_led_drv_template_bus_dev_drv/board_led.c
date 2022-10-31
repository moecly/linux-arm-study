#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

#include "led_resource.h"

#define SUCC 0
#define ERR -1

static struct resource led_resource[] = {
    {
        .start = GROUP_PIN(5, 3),
        .flags = IORESOURCE_IRQ,
        .name = "led_pin",
    },
    {
        .start = GROUP_PIN(3, 1),
        .flags = IORESOURCE_IRQ,
        .name = "led_pin",
    },
};

static struct platform_device led_device = {
    .name = "led",
    .resource = led_resource,
    .num_resources = ARRAY_SIZE(led_resource),
};

static int __init led_dev_init(void) {
  platform_device_register(&led_device);
  return SUCC;
}

static void __exit led_dev_exit(void) {
  platform_device_unregister(&led_device);
}

module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");