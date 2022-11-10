#include "asm/gpio.h"
#include "asm/uaccess.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/gpio_keys.h"
#include "linux/irqreturn.h"
#include "linux/kdev_t.h"
#include "linux/of.h"
#include "linux/poll.h"
#include "linux/printk.h"
#include "linux/wait.h"
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

#define BUF_LEN 120

typedef struct gpio_keys {
  int gpio;
  int flags;
  int irq;
  struct gpio_desc *desc;
} gpio_keys, *p_gpio_keys;

static p_gpio_keys g_p_keys;
static int g_key[BUF_LEN];
static int g_key_r, g_key_w;
static int major;
static struct class *keys_class;
static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);
static struct fasync_struct *key_fasync;

static int is_key_buf_empty(void) { return g_key_r == g_key_w; }

static int is_key_buf_full(void) { return g_key_r + 1 == BUF_LEN; }

static void put_key(int key) {
  if (!is_key_buf_full()) {
    g_key[g_key_w] = key;
    g_key_w = (g_key_w + 1) % BUF_LEN;
  }
}

static int get_key(void) {
  int key = -1;
  if (!is_key_buf_empty()) {
    key = g_key[g_key_r];
    g_key_r = (g_key_r + 1) % BUF_LEN;
  }
  return key;
}

static irqreturn_t keys_irq(int irq, void *dev_id) {
  int val;
  p_gpio_keys pkeys = (p_gpio_keys)dev_id;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  val = gpiod_get_value(pkeys->desc);
  put_key((g_p_keys->gpio << 8) | val);
  wake_up_interruptible(&gpio_key_wait);
  kill_fasync(&key_fasync, SIGIO, POLL_IN);

  return IRQ_HANDLED;
}

static ssize_t keys_read(struct file *file, char __user *buf, size_t size,
                         loff_t *offset) {
  int ret;
  int key;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  if (is_key_buf_empty() && (file->f_flags & O_NONBLOCK))
    return -EAGAIN;

  wait_event_interruptible(gpio_key_wait, !is_key_buf_empty());
  key = get_key();
  ret = copy_to_user(buf, &key, 4);
  return 4;
}

static unsigned int keys_poll(struct file *fd, struct poll_table_struct *wait) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  poll_wait(fd, &gpio_key_wait, wait);
  return is_key_buf_empty() ? 0 : POLLIN | POLLRDNORM;
}

static int keys_fasync(int fd, struct file *file, int on) {
  int ret;

  ret = fasync_helper(fd, file, on, &key_fasync);
  if (ret < 0) {
    return -EIO;
  }

  return 0;
}

static struct file_operations keys_fops = {
    .owner = THIS_MODULE,
    .read = keys_read,
    .poll = keys_poll,
    .fasync = keys_fasync,
};

int keys_probe(struct platform_device *pdev) {
  int i = 0;
  int count;
  int ret;
  enum of_gpio_flags flags;
  struct device_node *node;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
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
    ret = request_thread_irq(g_p_keys[i].irq, keys_irq,
                             IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                             "imx_keys", &g_p_keys[i]);
    if (ret)
      goto out;
  }

  /* 注册设备号 */
  major = register_chrdev(0, "keys", &keys_fops);
  if (major < 0)
    goto out;

  /* 注册class */
  keys_class = class_create(THIS_MODULE, "keys");
  if (IS_ERR(keys_class)) {
    unregister_chrdev(major, "keys");
    ret = PTR_ERR(keys_class);
    goto out;
  }

  /* 注册设备 */
  device_create(keys_class, NULL, MKDEV(major, 0), NULL, "key");

  return True;
out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return False;
}

int keys_remove(struct platform_device *pdev) {
  int i = 0;
  int count;
  struct device_node *node = pdev->dev.of_node;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  count = of_gpio_count(node);
  for (; i < count; i++) {
    /* 注销中断 */
    free_irq(g_p_keys[i].irq, &g_p_keys[i]);
  }

  device_destroy(keys_class, MKDEV(major, 0));
  class_destroy(keys_class);
  unregister_chrdev(major, "keys");
  kfree(g_p_keys);

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
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  platform_driver_register(&keys_driver);
  return 0;
}

/**
 * 退出
 */
static void __exit keys_exit(void) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  platform_driver_unregister(&keys_driver);
}

module_init(keys_init);
module_exit(keys_exit);
MODULE_LICENSE("GPL");