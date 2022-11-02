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
#include <linux/of.h>

#include "asm/io.h"
#include "led_ops.h"
#include "led_resource.h"
#include "leddrv.h"
#include "linux/ioport.h"
#include "linux/printk.h"

#define SUCC 0
#define ERR -1

static led_resource g_led_resource[100];
static int g_led_cnt = 0;

/**
 * init led
 */
int led_init(int which) {
  volatile unsigned char *ccm_ccgr;
  volatile unsigned int *iomuxc;
  volatile unsigned int *gdir;
  ccm_ccgr = ioremap(g_led_resource[which].ccm_ccgr, sizeof(char));
  iomuxc = ioremap(g_led_resource[which].iomuxc, sizeof(int));
  gdir = ioremap(g_led_resource[which].gdir, sizeof(int));

  printk("init gpio: gruop %d, pin %d\n", GROUP(g_led_resource[which].pin),
         PIN(g_led_resource[which].pin));

  *iomuxc &= ~0xf;
  *iomuxc |= 0x5;
  *gdir |= (1 << PIN(g_led_resource[which].pin));

  iounmap(iomuxc);
  iounmap(gdir);
  return SUCC;
}

/**
 * set led status
 */
int led_ctl(int which, char status) {
  volatile unsigned int *dr;
  dr = ioremap(g_led_resource[which].dr, sizeof(int));

  printk("ctl gpio: gruop %d, pin %d\n", GROUP(g_led_resource[which].pin),
         PIN(g_led_resource[which].pin));

  if (status) {
    printk("on.");
    *dr &= ~(1 << PIN(g_led_resource[which].pin));
  } else {
    printk("off.");
    *dr |= (1 << PIN(g_led_resource[which].pin));
  }

  iounmap(dr);
  return SUCC;
}

/**
 * toggle led
 */
int led_toggle(int which) {
  volatile unsigned int *dr;
  int pin;
  pin = PIN(g_led_resource[which].pin);
  dr = ioremap(g_led_resource[which].dr, sizeof(int));

  printk("toggle gpio: gruop %d, pin %d\n", GROUP(g_led_resource[which].pin),
         PIN(g_led_resource[which].pin));

  if (*dr & (1 << pin))
    *dr &= ~(1 << pin);
  else
    *dr |= 1 << pin;
  return SUCC;
}

led_operations led_ops = {
    .ctl = led_ctl,
    .init = led_init,
    .toggle = led_toggle,
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
  err = of_property_read_u32(dev_node, "ccm_ccgr", &res);
  if (err == ERR)
    return err;
  led_res.ccm_ccgr = res;

  /* 获取iomuxc */
  err = of_property_read_u32(dev_node, "iomuxc", &res);
  if (err == ERR)
    return err;
  led_res.iomuxc = res;

  /* 获取dr */
  err = of_property_read_u32(dev_node, "dr", &res);
  if (err == ERR)
    return err;
  led_res.dr = res;

  /* 获取gdir */
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