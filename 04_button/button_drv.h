#ifndef __BUTTON_DRV_H__
#define __BUTTON_DRV_H__

#define GROUP(g) (g >> 16)
#define PIN(p) (p & 0xffff)
#define GROUP_PIN(g, p) ((g << 16) | p)

/**
 * button opreations
 */
typedef struct button_operations {
  int (*init)(int);
  int (*read)(int);
} button_operations, *p_button_operations;

/**
 * led resource
 */
typedef struct button_resource {
  unsigned int ccm_ccgr;
  unsigned int ccm_offset;
  unsigned int iomuxc;
  unsigned int gpio;
  unsigned int pin;
} button_resource, *p_button_resource;

/**
 * gpio
 */
struct gpio {
  volatile unsigned int dr;
  volatile unsigned int gdir;
};

p_button_operations get_button_ops(void);

#endif