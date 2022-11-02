#ifndef _LED_RESOURCE_H
#define _LED_RESOURCE_H

/* GPIO3_0 */
/* bit[31:16] = group */
/* bit[15:0]  = which pin */
#define GROUP(x) (x >> 16)
#define PIN(x) (x & 0xFFFF)
#define GROUP_PIN(g, p) ((g << 16) | (p))

typedef struct led_resource {
  int pin;
  volatile unsigned int dr;
  volatile unsigned int gdir;
  volatile unsigned char ccm_ccgr;
  volatile unsigned int iomuxc;
} led_resource, *p_led_resource;

#endif
