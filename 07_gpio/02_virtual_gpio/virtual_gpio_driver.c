#include "linux/device.h"
#include "linux/export.h"
#include "linux/gfp.h"
#include "linux/platform_device.h"
#include "linux/printk.h"
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>

static struct gpio_chip *chip;
static int gpio_val = 0;

static int gpio_direction_input(struct gpio_chip *chip, unsigned offset) {
  printk("gpio direction input\n");
  return 0;
}

static int gpio_direction_output(struct gpio_chip *chip, unsigned offset,
                                 int value) {
  printk("gpio direction output\n");
  return 0;
}

static int gpio_get(struct gpio_chip *chip, unsigned offset) {
  int val;
  val = (gpio_val & (1 << offset)) ? 1 : 0;
  printk("gpio get val = %d\n", val);
  return val;
}

static void gpio_set(struct gpio_chip *chip, unsigned offset, int value) {
  printk("gpio set val = %d\n", value);
  if (value)
    gpio_val |= (1 << offset);
  else
    gpio_val &= ~(1 << offset);
}

static int virtual_gpio_probe(struct platform_device *pdev) {
  struct device_node *node = pdev->dev.of_node;
  int i = 0;
  int ngpios;

  of_property_read_u32(node, "ngpios", &ngpios);
  if (ngpios < 0)
    goto err_get_gpio_number;

  chip =
      devm_kzalloc(&pdev->dev, ngpios * sizeof(struct gpio_chip), GFP_KERNEL);

  gpiochip_add_data(chip, NULL);
  chip->base = -1;
  chip->parent = &pdev->dev;
  chip->owner = THIS_MODULE;
  chip->direction_input = gpio_direction_input;
  chip->direction_output = gpio_direction_output;
  chip->get = gpio_get;
  chip->set = gpio_set;

  return 0;

err_get_gpio_number:
  return -1;
}

static int virtual_gpio_remove(struct platform_device *pdev) {
  gpiochip_remove(chip);
  return 0;
}

static struct of_device_id virtual_imx6ul_pinctrl_of_match[] = {
    {
        .compatible = "100ask,virtual_pinctrl",
    },
    {/* sentinel */},
};

static struct platform_driver virtual_gpio_driver = {
    .driver =
        {
            .name = "virtual_gpio_driver",
            .of_match_table = of_match_ptr(virtual_imx6ul_pinctrl_of_match),
        },
    .probe = virtual_gpio_probe,
    .remove = virtual_gpio_remove,
};

module_platform_driver(virtual_gpio_driver);

MODULE_LICENSE("GPL");
