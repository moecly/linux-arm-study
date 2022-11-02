#ifndef _LED_OPS_H
#define _LED_OPS_H

typedef struct led_operations {
  int (*init)(int which); /* 初始化LED, which-哪个LED */
  int (*toggle)(int which);
  int (*ctl)(int which,
             char status); /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
} led_operations, *p_led_operations;

struct led_operations *get_led_ops(void);

#endif
