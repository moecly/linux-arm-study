#ifndef _LEDDRV_H
#define _LEDDRV_H

#include "led_ops.h"

void led_class_create_dev(int minor);
void led_class_destory_dev(int minor);
void register_led_operations(p_led_operations p_ops);

#endif /* _LEDDRV_H */
