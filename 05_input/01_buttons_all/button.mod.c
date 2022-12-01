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
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0xff178f6, __VMLINUX_SYMBOL_STR(__aeabi_idivmod) },
	{ 0xb678eb47, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x52bfdf34, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xbd725d69, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x62451f4, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0x5ee52022, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x1b694d4f, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x530224b5, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xdc17d938, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x8f22d2ef, __VMLINUX_SYMBOL_STR(gpiod_get_value) },
	{ 0xa38caae0, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x77d800a2, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x87ffdb4a, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x5c2e3421, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0x5a658f4f, __VMLINUX_SYMBOL_STR(of_count_phandle_with_args) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

