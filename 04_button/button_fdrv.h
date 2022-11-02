#ifndef __BUTTON_FDRV_H__
#define __BUTTON_FDRV_H__

#include "button_drv.h"
void create_device_from_class(int minor);
void destroy_device_from_class(int minor);
void set_button_ops(p_button_operations p_button_ops);

#endif