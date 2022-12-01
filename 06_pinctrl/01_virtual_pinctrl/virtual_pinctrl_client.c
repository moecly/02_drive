#include "linux/printk.h"
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/regmap.h>
#include <linux/slab.h>

static const struct of_device_id virtual_client_of_match[] = {
    {
        .compatible = "100ask,virtual_i2c",
    },
    {},
};

static int virtual_imx6ul_pinctrl_client_probe(struct platform_device *pdev) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static int virtual_imx6ul_pinctrl_client_remove(struct platform_device *pdev) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static struct platform_driver virtual_imx6ul_pinctrl_client_driver = {
    .driver =
        {
            .name = "virtual-imx6ul-pinctrl-client",
            .of_match_table = of_match_ptr(virtual_client_of_match),
        },
    .probe = virtual_imx6ul_pinctrl_client_probe,
    .remove = virtual_imx6ul_pinctrl_client_remove,
};

module_platform_driver(virtual_imx6ul_pinctrl_client_driver);
MODULE_LICENSE("GPL");