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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

#include "asm/uaccess.h"
#include "button_drv.h"
#include "button_fdrv.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/kdev_t.h"
#include "linux/printk.h"

#define SUCC 0
#define ERR -1

static int major = 0;
static struct class *g_p_button_class;
static p_button_operations g_p_button_ops;

void create_device_from_class(int minor) {
  device_create(g_p_button_class, NULL, MKDEV(major, minor), NULL, "button%d",
                minor);
}

void destroy_device_from_class(int minor) {
  device_destroy(g_p_button_class, MKDEV(major, minor));
}

void set_button_ops(p_button_operations p_button_ops) {
  g_p_button_ops = p_button_ops;
}

EXPORT_SYMBOL(create_device_from_class);
EXPORT_SYMBOL(destroy_device_from_class);
EXPORT_SYMBOL(set_button_ops);

/**
 * 读取按键
 */
static ssize_t button_read(struct file *file, char __user *buf, size_t size,
                           loff_t *offset) {
  struct inode *node;
  int minor;

  if (!g_p_button_ops) {
    printk("button opfs is null\n");
    return ERR;
  }

  node = file_inode(file);
  minor = iminor(node);
  g_p_button_ops->read(minor);
  return SUCC;
}

/**
 * 打开按键驱动
 */
static int button_open(struct inode *node, struct file *file) {
  int minor;

  if (!g_p_button_ops) {
    printk("button opfs is null\n");
    return ERR;
  }

  minor = iminor(node);
  g_p_button_ops->init(minor);
  return SUCC;
}

/**
 * 关闭驱动
 */
static int button_close(struct inode *node, struct file *file) { return SUCC; }

static struct file_operations button_fops = {
    .owner = THIS_MODULE,
    .open = button_open,
    .read = button_read,
    .release = button_close,
};

static int __init button_fdrv_init(void) {
  /* 注册设备号 */
  major = register_chrdev(0, "button", &button_fops);
  if (major < 0) {
    printk("major register failed\n");
    return major;
  }

  /* 注册类 */
  g_p_button_class = class_create(THIS_MODULE, "button");
  if (IS_ERR(g_p_button_class)) {
    printk("class create failed\n");
    return PTR_ERR(g_p_button_class);
  }

  return SUCC;
}

static void __exit button_fdrv_exit(void) {
  /* 注销类 */
  class_destroy(g_p_button_class);
  /* 注销设备号 */
  unregister_chrdev(major, "button");
}

module_init(button_fdrv_init);
module_exit(button_fdrv_exit);
MODULE_LICENSE("GPL");