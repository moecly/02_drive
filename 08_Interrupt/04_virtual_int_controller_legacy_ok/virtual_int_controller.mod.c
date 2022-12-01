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
	{ 0xc03430f6, __VMLINUX_SYMBOL_STR(irq_domain_xlate_onetwocell) },
	{ 0xd6c4624b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xd49c9dee, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x7ceaf0d5, __VMLINUX_SYMBOL_STR(generic_handle_irq) },
	{ 0xd5de4450, __VMLINUX_SYMBOL_STR(irq_find_mapping) },
	{ 0xf09de776, __VMLINUX_SYMBOL_STR(get_random_int) },
	{ 0x7940b20d, __VMLINUX_SYMBOL_STR(irq_set_chip_and_handler_name) },
	{ 0xc1cab82e, __VMLINUX_SYMBOL_STR(handle_level_irq) },
	{ 0x20a789ac, __VMLINUX_SYMBOL_STR(irq_set_chip_data) },
	{ 0xf11a12ed, __VMLINUX_SYMBOL_STR(irq_domain_add_legacy) },
	{ 0x8777df7d, __VMLINUX_SYMBOL_STR(__irq_alloc_descs) },
	{ 0xef28bde, __VMLINUX_SYMBOL_STR(irq_set_chained_handler_and_data) },
	{ 0xed2c97a9, __VMLINUX_SYMBOL_STR(platform_get_irq) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

