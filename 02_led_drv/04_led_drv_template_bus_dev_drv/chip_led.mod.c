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
	{ 0x3cb79823, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xd6c4624b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xd49c9dee, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0xc1e817c1, __VMLINUX_SYMBOL_STR(led_class_destory_dev) },
	{ 0x4a2c9e06, __VMLINUX_SYMBOL_STR(led_class_create_dev) },
	{ 0x2d9b5b90, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=leddrv";

