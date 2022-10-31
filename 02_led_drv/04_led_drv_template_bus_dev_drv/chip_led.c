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

#include "led_ops.h"
#include "led_resource.h"
#include "leddrv.h"
#include "linux/ioport.h"
#include "linux/printk.h"

#define SUCC 0
#define ERR -1

static int g_led_pins[100];
static int g_led_cnt = 0;

/**
 * init led
 */
// TODO
int led_init(int which) {
  printk("init gpio: gruop %d, pin %d\n", GROUP(g_led_pins[which]),
         PIN(g_led_pins[which]));
  return SUCC;
}

/**
 * set led status
 */
// TODO
int led_ctl(int which, char status) {
  printk("ctl gpio: gruop %d, pin %d\n", GROUP(g_led_pins[which]),
         PIN(g_led_pins[which]));
  return SUCC;
}

led_operations led_ops = {
    .ctl = led_ctl,
    .init = led_init,
};

struct led_operations *get_led_ops(void) {
  return &led_ops;
}

/**
 * led drv probe function
 */
int led_drv_probe(struct platform_device *dev) {
  /* 记录引脚 */
  int i = 0;
  struct resource *res;

  while (1) {
    res = platform_get_resource(dev, IORESOURCE_IRQ, i++);

    if (!res)
      break;

    led_class_create_dev(g_led_cnt);
    g_led_pins[g_led_cnt++] = res->start;
  }

  return SUCC;
}

/**
 * led drv probe remove
 */
int led_drv_remove(struct platform_device *dev) {
  int i = 0;
  struct resource *res;

  while (1) {
    res = platform_get_resource(dev, IORESOURCE_IRQ, i);

    if (!res)
      break;

    led_class_destory_dev(i);
    g_led_cnt--;
  }

  return SUCC;
}

static struct platform_driver led_driver = {
    .probe = led_drv_probe,
    .remove = led_drv_remove,
    .driver =
        {
            .name = "led",
        },
};

/**
 * 入口函数
 */
static int __init led_drv_init(void) {
  platform_driver_register(&led_driver);
  /* 底层注册 */
  register_led_operations(&led_ops);
  return SUCC;
}

/**
 * 出口函数
 */
static void __exit led_drv_exit(void) {
  platform_driver_unregister(&led_driver);
}

module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_LICENSE("GPL");