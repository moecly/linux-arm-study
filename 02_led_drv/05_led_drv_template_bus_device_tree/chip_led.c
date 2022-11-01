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
#include "linux/of.h"
#include "linux/printk.h"

#define SUCC 0
#define ERR -1

static led_resource g_led_resource[100];
static int g_led_cnt = 0;

/**
 * init led
 */
// TODO
int led_init(int which) {
  printk("ctl gpio: gruop %d, pin %d\n", GROUP(g_led_resource[which].pin),
         PIN(g_led_resource[which].pin));
  return SUCC;
}

/**
 * set led status
 */
// TODO
int led_ctl(int which, char status) {
  printk("ctl gpio: gruop %d, pin %d\n", GROUP(g_led_resource[which].pin),
         PIN(g_led_resource[which].pin));
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
  int res;
  int err;
  struct device_node *dev_node;
  led_resource led_res;

  /* 找不到device_node返回err */
  dev_node = dev->dev.of_node;
  if (!dev_node)
    return ERR;

  /* 获取pin */
  err = of_property_read_u32(dev_node, "pin", &res);
  if (err == ERR)
    return err;
  led_res.pin = res;

  /* 获取ccm */
  err = of_property_read_u32(dev_node, "ccm", &res);
  if (err == ERR)
    return err;
  led_res.ccm = res;

  /* 获取dr */
  err = of_property_read_u32(dev_node, "dr", &res);
  if (err == ERR)
    return err;
  led_res.dr = res;

  /* 获取dr */
  err = of_property_read_u32(dev_node, "gdir", &res);
  if (err == ERR)
    return err;
  led_res.gdir = res;

  /* 创建设备节点 */
  led_class_create_dev(g_led_cnt);
  g_led_resource[g_led_cnt++] = led_res;
  return SUCC;
}

/**
 * led drv probe remove
 */
int led_drv_remove(struct platform_device *dev) {
  int i = 0;
  int res;
  int err;
  struct device_node *dev_node;
  led_resource led_res;

  /* 判断节点是否为空 */
  dev_node = dev->dev.of_node;
  if (!dev_node)
    return ERR;

  /* 读取pin */
  err = of_property_read_u32(dev_node, "pin", &res);
  if (err == ERR)
    return err;
  led_res.pin = res;

  /* 获取ccm */
  err = of_property_read_u32(dev_node, "ccm", &res);
  if (err == ERR)
    return err;
  led_res.ccm = res;

  /* 获取dr */
  err = of_property_read_u32(dev_node, "dr", &res);
  if (err == ERR)
    return err;
  led_res.dr = res;

  /* 获取dr */
  err = of_property_read_u32(dev_node, "gdir", &res);
  if (err == ERR)
    return err;
  led_res.gdir = res;

  /* 查找要烧毁的设备 */
  for (; i < g_led_cnt; i++) {
    if (g_led_resource[i].pin == led_res.pin) {
      led_class_create_dev(i);
      g_led_resource[i].pin = -1;
      break;
    }
  }

  for (i = 0; i < g_led_cnt; i++) {
    if (g_led_resource[i].pin != -1)
      break;
  }

  if (i == g_led_cnt)
    g_led_cnt = 0;

  return SUCC;
}

static const struct of_device_id imx6ull_led[] = {
    {.compatible = "imx6ull, led_drv"},
    {},
};

static struct platform_driver led_driver = {
    .probe = led_drv_probe,
    .remove = led_drv_remove,
    .driver =
        {
            .name = "led",
            .of_match_table = imx6ull_led,
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