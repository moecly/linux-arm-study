cmd_/linux/drive/02_led_drv/05_led_drv_template_bus_device_tree/leddrv.ko := arm-linux-gnueabihf-ld -EL -r  -T ./scripts/module-common.lds --build-id  -o /linux/drive/02_led_drv/05_led_drv_template_bus_device_tree/leddrv.ko /linux/drive/02_led_drv/05_led_drv_template_bus_device_tree/leddrv.o /linux/drive/02_led_drv/05_led_drv_template_bus_device_tree/leddrv.mod.o ;  true