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
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

#include "asm/io.h"
#include "button_drv.h"
#include "button_fdrv.h"
#include "linux/export.h"
#include "linux/iommu.h"
#include "linux/printk.h"
#include "linux/types.h"
#include "my_delay.h"

#define SUCC 0
#define ERR -1

static button_resource button_res[100];
static int button_cnt = 0;

static int button_init(int which) {
  volatile unsigned int *iomuxc;
  volatile unsigned int *ccm_ccgr;
  volatile struct gpio *gpio;
  // volatile unsigned int *ccm_offset;

  /* 映射引脚 */
  iomuxc = ioremap(button_res[which].iomuxc, sizeof(int));
  gpio = ioremap(button_res[which].gpio, sizeof(struct gpio));
  ccm_ccgr = ioremap(button_res[which].ccm_ccgr, sizeof(int));
  // ccm_offset = ioremap(button_res[which].ccm_offset, sizeof(int));

  /* 配置gpio */
  *iomuxc &= ~(0xf);
  *iomuxc |= 0x5;
  // *ccm_ccgr |= (0x3 << *ccm_offset);
  gpio->gdir &= ~(1 << PIN(button_res[which].pin));

  iounmap(iomuxc);
  iounmap(gpio);
  iounmap(ccm_ccgr);
  return SUCC;
}

static int button_read(int which) {
  volatile struct gpio *gpio;

  /* 读取数据 */
  gpio = ioremap(button_res[which].gpio, sizeof(struct gpio));
  if (!((gpio->dr >> PIN(button_res[which].pin)) & 0x1)) {
    printk("key%d on\n", which);
    while (!((gpio->dr >> PIN(button_res[which].pin)) & 0x1)) {
    }
    delay_ms(10);
  }

  iounmap(gpio);
  return SUCC;
}

static button_operations button_ops = {
    .init = button_init,
    .read = button_read,
};

p_button_operations get_button_ops(void) { return &button_ops; }

static int button_probe(struct platform_device *pdev) {
  struct device_node *d_node;
  button_resource b_res;
  int res;
  int err;

  d_node = pdev->dev.of_node;
  if (!d_node) {
    printk("device node is null\n");
    return ERR;
  }

  /* 读取pin */
  err = of_property_read_u32(d_node, "pin", &res);
  printk("probe pin: %d\n", res);
  if (err < 0) {
    return ERR;
  }
  b_res.pin = res;

  /* 读取gpio */
  err = of_property_read_u32(d_node, "gpio", &res);
  if (err < 0) {
    return ERR;
  }
  b_res.gpio = res;

  /* 读取ccm_offset */
  err = of_property_read_u32(d_node, "ccm_offset", &res);
  if (err < 0) {
    return ERR;
  }
  b_res.ccm_offset = res;

  /* 读取iomuxc */
  err = of_property_read_u32(d_node, "iomuxc", &res);
  if (err < 0) {
    return ERR;
  }
  b_res.iomuxc = res;

  /* 读取ccm_ccgr */
  err = of_property_read_u32(d_node, "ccm_ccgr", &res);
  if (err < 0) {
    return ERR;
  }
  b_res.ccm_ccgr = res;

  create_device_from_class(button_cnt);
  button_res[button_cnt++] = b_res;
  return SUCC;
}

static int button_remove(struct platform_device *pdev) {
  struct device_node *d_node;
  int pin;
  int err;
  char can_find;
  int i = 0;

  d_node = pdev->dev.of_node;
  err = of_property_read_u32(d_node, "pin", &pin);
  if (err < 0) {
    return ERR;
  }

  /* 查找要移除的gpio */
  can_find = 0;
  for (; i < button_cnt; i++) {
    if (button_res[i].pin == -1)
      continue;

    if (button_res[i].pin == pin) {
      button_res[i].pin = -1;

      if (can_find)
        break;
    } else {
      can_find = 1;
    }
  }

  /* 找不到就清零 */
  if (!can_find) {
    button_cnt = 0;
  }

  return SUCC;
}

static struct of_device_id imx_button[] = {
    {
        .compatible = "imx6ull, button_drv",
    },
    {/* sentinel */}};

static struct platform_driver button_driver = {
    .probe = button_probe,
    .remove = button_remove,
    .driver =
        {
            .name = "button",
            .of_match_table = imx_button,
        },
};

static int __init button_drv_init(void) {
  platform_driver_register(&button_driver);
  set_button_ops(&button_ops);
  return SUCC;
}

static void __exit button_drv_exit(void) {
  platform_driver_unregister(&button_driver);
}

module_init(button_drv_init);
module_exit(button_drv_exit);
MODULE_LICENSE("GPL");