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
	{ 0xeeda1a2b, __VMLINUX_SYMBOL_STR(of_property_read_u32_index) },
	{ 0x8f821c5b, __VMLINUX_SYMBOL_STR(of_property_read_string_helper) },
	{ 0x452238c2, __VMLINUX_SYMBOL_STR(devm_pinctrl_register) },
	{ 0x59e3f924, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x79c91e05, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

