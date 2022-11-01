#include <linux/module.h>

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

#include "asm/uaccess.h"
#include "led_ops.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/printk.h"

/* 1. 确定主设备号 */
static int major = 0;
static struct class *led_class;
p_led_operations p_led_ops;

#define ERR -1
#define SUCC 0

#define MIN(a, b) (a < b ? a : b)

/**
 * create led dev
 */
void led_class_create_dev(int minor) {
  device_create(led_class, NULL, MKDEV(major, minor), "100ask_led%d", minor);
}

/**
 * destory led dev
 */
void led_class_destroy_dev(int minor) {
  device_destroy(led_class, MKDEV(major, minor));
}

/**
 * 由底层调用注册
 */
void register_led_operations(p_led_operations p_ops) { p_led_ops = p_ops; }

/* ko文件需要引用，需导出 */
EXPORT_SYMBOL(led_class_create_dev);
EXPORT_SYMBOL(led_class_destroy_dev);
EXPORT_SYMBOL(register_led_operations);

/**
 * led read function
 */
ssize_t led_read(struct file *file, char __user *buf, size_t size,
                 loff_t *offset) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return SUCC;
}

/**
 * led write function
 */
ssize_t led_write(struct file *file, const char __user *buf, size_t size,
                  loff_t *offset) {
  char val;
  int res;
  int minor;
  struct inode *node;

  if (size != 1 || !p_led_ops)
    goto out;

  /* 显示led */
  node = file_inode(file);
  res = copy_from_user(&val, buf, size);
  minor = iminor(node);
  p_led_ops->ctl(minor, val);

  return size;

out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return ERR;
}

/**
 * led close function
 */
int led_close(struct inode *node, struct file *file) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return SUCC;
}

/**
 * led open function
 */
int led_open(struct inode *node, struct file *file) {
  /* configure led gpio */
  if (!p_led_ops)
    goto out;

  int minor = iminor(node);
  p_led_ops->init(minor);
  return SUCC;

out:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return ERR;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_close,
};

int __init led_init(void) {
  /* 注册major号 */
  major = register_chrdev(0, "led", &led_fops);
  if (major < 0) {
    printk(KERN_ERR "gpio: couldn't get a major number.\n");
    return major;
  }

  /* 创建一个led的class */
  led_class = class_create(THIS_MODULE, "led");
  if (IS_ERR(led_class))
    return PTR_ERR(led_class);

  /* 存在交叉依赖，使用这个函数需要加载另外一个文件，而使用另外一个文件又需要使用这个文件的函数,
    可以由底层主动提供
   */
  // p_led_ops = get_led_ops();

  return SUCC;
}

void __exit led_exit(void) {
  class_destroy(led_class);
  unregister_chrdev(major, "led");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
