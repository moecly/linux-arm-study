#include "asm/gpio.h"
#include "linux/gpio_keys.h"
#include "linux/irqreturn.h"
#include "linux/of.h"
#include "linux/printk.h"
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/tty.h>

#define True 0
#define False -1

typedef struct gpio_keys {
  int gpio;
  int flags;
  int irq;
  struct gpio_desc *desc;
} gpio_keys, *p_gpio_keys;

p_gpio_keys g_p_keys;

static irqreturn_t keys_irq(int irq, void *dev_id) {
  int val;
  p_gpio_keys pkeys = (p_gpio_keys)dev_id;

  val = gpiod_get_value(pkeys->desc);
  printk("key %d %d\n", pkeys->gpio, val);

  return IRQ_HANDLED;
}

// TODO
int keys_probe(struct platform_device *pdev) {
  int i = 0;
  int count;
  int ret;
  enum of_gpio_flags flags;
  struct device_node *node;

  node = pdev->dev.of_node;
  count = of_gpio_count(node);
  if (count <= 0) {
    goto out;
  }

  g_p_keys = kzalloc(sizeof(gpio_keys) * count, GFP_KERNEL);
  for (; i < count; i++) {
    g_p_keys[i].gpio = of_get_gpio_flags(node, i, &flags);
    if (g_p_keys[i].gpio < 0) {
      goto out;
    }

    g_p_keys[i].desc = gpio_to_desc(g_p_keys[i].gpio);
    g_p_keys[i].irq = gpio_to_irq(g_p_keys[i].gpio);
    g_p_keys[i].flags = flags & OF_GPIO_ACTIVE_LOW;
    ret = request_irq(g_p_keys[i].irq, keys_irq,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "imx_keys",
                      &g_p_keys[i]);
    if (ret)
      goto out;
  }

  return True;
out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return False;
}

// TODO
int keys_remove(struct platform_device *pdev) {
  int i = 0;
  int count;
  struct device_node *node = pdev->dev.of_node;

  count = of_gpio_count(node);
  for (; i < count; i++) {
    free_irq(i, &g_p_keys[i]);
  }

  return True;
}

static const struct of_device_id imx6_keys[] = {
    {.compatible = "imx,keys"},
    {},
};

static struct platform_driver keys_driver = {
    .probe = keys_probe,
    .remove = keys_remove,
    .driver =
        {
            .name = "keys",
            .of_match_table = imx6_keys,
        },
};

/**
 * 初始化
 */
static int __init keys_init(void) {
  platform_driver_register(&keys_driver);
  return 0;
}

/**
 * 退出
 */
static void __exit keys_exit(void) { platform_driver_unregister(&keys_driver); }

module_init(keys_init);
module_exit(keys_exit);
MODULE_LICENSE("GPL");