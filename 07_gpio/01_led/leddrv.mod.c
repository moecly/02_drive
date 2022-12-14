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
	{ 0xddd06ce6, __VMLINUX_SYMBOL_STR(no_llseek) },
	{ 0xd6c4624b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xd49c9dee, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb678eb47, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x52bfdf34, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xbd725d69, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xd3c8c747, __VMLINUX_SYMBOL_STR(devm_gpiod_get_index) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x6aa6ada8, __VMLINUX_SYMBOL_STR(gpiod_set_value) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0xa19fd89, __VMLINUX_SYMBOL_STR(gpiod_direction_output) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x77d800a2, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x87ffdb4a, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

