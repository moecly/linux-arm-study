#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9a454969, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x762124b9, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x8b93010a, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x7701869e, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x76596166, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x7c6db3b0, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x9fb4c6bd, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xcc432362, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x5278a4fb, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xf0f4ffad, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xba3179a6, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xdbb1beb5, __VMLINUX_SYMBOL_STR(of_count_phandle_with_args) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x763e342d, __VMLINUX_SYMBOL_STR(gpiod_get_value) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8FB00160A4FF47AA961F60C");
