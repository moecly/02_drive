#include <linux/device.h>
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

#include "core.h"
#include "linux/gfp.h"
#include "linux/printk.h"

static const struct pinctrl_pin_desc imx6ul_pinctrl_pads[] = {
    {0, "pin0", NULL},
    {1, "pin1", NULL},
    {2, "pin2", NULL},
    {3, "pin3", NULL},
};

struct virtual_functions_desc {
  const char *func_name;
  const char **groups;
  int num_groups;
};

static const char *func0_grps[] = {"pin0", "pin1", "pin2", "pin3"};
static const char *func1_grps[] = {"pin0", "pin1"};
static const char *func2_grps[] = {"pin2", "pin3"};

static unsigned long pin_configs[4];

static struct virtual_functions_desc g_funcs_des[] = {
    {"gpio", func0_grps, 4},
    {"i2c", func1_grps, 2},
    {"uart", func2_grps, 2},
};

static struct of_device_id virtual_imx6ul_pinctrl_of_match[] = {
    {
        .compatible = "100ask,virtual_pinctrl",
    },
    {/* sentinel */},
};

static int imx_get_groups_count(struct pinctrl_dev *pctldev) {
  return pctldev->desc->npins;
}

static const char *imx_get_group_name(struct pinctrl_dev *pctldev,
                                      unsigned selector) {
  return pctldev->desc->pins[selector].name;
}

static int imx_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
                              const unsigned **pins, unsigned *num_pins) {
  if (selector > pctldev->desc->npins)
    return -EINVAL;

  *pins = &pctldev->desc->pins[selector].number;
  *num_pins = 1;
  return 0;
}

static void imx_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
                             unsigned offset) {
  seq_printf(s, "%s", dev_name(pctldev->dev));
}

static int imx_dt_node_to_map(struct pinctrl_dev *pctldev,
                              struct device_node *np_config,
                              struct pinctrl_map **map, unsigned *num_maps) {

  struct pinctrl_map *new_map;
  const char *pins;
  const char *function;
  unsigned long *configs;
  unsigned int config;
  int i = 0;
  int num_pins = 0;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  for (;;) {
    if (of_property_read_string_index(np_config, "group", num_pins, &pins) == 0)
      num_pins++;
    else
      break;
  }

  new_map = devm_kzalloc(pctldev->dev,
                         sizeof(struct pinctrl_map) * num_pins * 2, GFP_KERNEL);
  if (!new_map)
    return -EINVAL;

  for (; i < num_pins; i++) {
    of_property_read_string_index(np_config, "group", i, &pins);
    of_property_read_string_index(np_config, "function", i, &function);
    of_property_read_u32_index(np_config, "configs", i, &config);

    new_map[i * 2].type = PIN_MAP_TYPE_MUX_GROUP;
    new_map[i * 2].data.mux.group = pins;
    new_map[i * 2].data.mux.function = function;

    configs = devm_kzalloc(pctldev->dev, sizeof(unsigned long), GFP_KERNEL);
    new_map[i * 2 + 1].type = PIN_MAP_TYPE_CONFIGS_PIN;
    new_map[i * 2 + 1].data.configs.num_configs = 1;
    new_map[i * 2 + 1].data.configs.group_or_pin = pins;
    new_map[i * 2 + 1].data.configs.configs = configs;
    *configs = (unsigned long)config;
  }

  *map = new_map;
  *num_maps = num_pins * 2;

  return 0;
}

static void imx_dt_free_map(struct pinctrl_dev *pctldev,
                            struct pinctrl_map *map, unsigned num_maps) {
  // kfree(map);
}

static const struct pinctrl_ops imx_pctrl_ops = {
    .get_groups_count = imx_get_groups_count,
    .get_group_name = imx_get_group_name,
    .get_group_pins = imx_get_group_pins,
    .pin_dbg_show = imx_pin_dbg_show,
    .dt_node_to_map = imx_dt_node_to_map,
    .dt_free_map = imx_dt_free_map,
};

static int imx_pmx_get_funcs_count(struct pinctrl_dev *pctldev) {
  return ARRAY_SIZE(g_funcs_des);
}

static const char *imx_pmx_get_func_name(struct pinctrl_dev *pctldev,
                                         unsigned selector) {
  return g_funcs_des[selector].func_name;
}

static int imx_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
                              const char *const **groups,
                              unsigned *num_groups) {
  *num_groups = g_funcs_des[selector].num_groups;
  *groups = g_funcs_des[selector].groups;
  return 0;
}

static int imx_pmx_set(struct pinctrl_dev *pctldev, unsigned func_selector,
                       unsigned group_selector) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static int imx_pmx_gpio_set_direction(struct pinctrl_dev *pctldev,
                                      struct pinctrl_gpio_range *range,
                                      unsigned offset, bool input) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static const struct pinmux_ops imx_pmx_ops = {
    .get_functions_count = imx_pmx_get_funcs_count,
    .get_function_name = imx_pmx_get_func_name,
    .get_function_groups = imx_pmx_get_groups,
    .set_mux = imx_pmx_set,
    .gpio_set_direction = imx_pmx_gpio_set_direction,
};

static int imx_pinconf_get(struct pinctrl_dev *pctldev, unsigned pin,
                           unsigned long *config) {
  printk("pinconf get\n");
  *config = pin_configs[pin];
  return 0;
}

static int imx_pinconf_set(struct pinctrl_dev *pctldev, unsigned pin,
                           unsigned long *configs, unsigned num_configs) {
  printk("num_configs = %d\n", num_configs);
  printk("pinconf set\n");
  pin_configs[pin] = *configs;
  printk("pin = %d, pin_config = %ld, pin_configs = %ld\n", pin,
         pin_configs[pin], *configs);
  return 0;
}

static void imx_pinconf_dbg_show(struct pinctrl_dev *pctldev,
                                 struct seq_file *s, unsigned offset) {
  seq_printf(s, "0x%lx", pin_configs[offset]);
}

static void imx_pinconf_group_dbg_show(struct pinctrl_dev *pctldev,
                                       struct seq_file *s, unsigned selector) {
  seq_printf(s, "0x%lx", pin_configs[selector]);
}

static const struct pinconf_ops imx_pinconf_ops = {
    .pin_config_get = imx_pinconf_get,
    .pin_config_set = imx_pinconf_set,
    .pin_config_dbg_show = imx_pinconf_dbg_show,
    .pin_config_group_dbg_show = imx_pinconf_group_dbg_show,
};

static int virtual_imx6ul_pinctrl_probe(struct platform_device *pdev) {
  struct pinctrl_desc *imx_pinctrl_desc;

  imx_pinctrl_desc =
      devm_kzalloc(&pdev->dev, sizeof(struct pinctrl_desc), GFP_KERNEL);
  if (!imx_pinctrl_desc) {
    printk("kzalloc imx pinctrl desc err\n");
    goto err_dvem_kzalloc_pinctrl_desc;
  }

  /*
   * config pinctrl_desc.
   */
  imx_pinctrl_desc->name = dev_name(&pdev->dev);
  imx_pinctrl_desc->pins = imx6ul_pinctrl_pads;
  imx_pinctrl_desc->npins = ARRAY_SIZE(imx6ul_pinctrl_pads);
  imx_pinctrl_desc->owner = THIS_MODULE;
  imx_pinctrl_desc->pctlops = &imx_pctrl_ops;
  imx_pinctrl_desc->pmxops = &imx_pmx_ops;
  imx_pinctrl_desc->confops = &imx_pinconf_ops;

  /*
   * register pinctrl.
   */
  devm_pinctrl_register(&pdev->dev, imx_pinctrl_desc, NULL);

  return 0;
err_dvem_kzalloc_pinctrl_desc:
  return -ENOMEM;
}

static struct platform_driver virtual_imx6ul_pinctrl_driver = {
    .driver =
        {
            .name = "virtual-imx6ul-pinctrl",
            .of_match_table = of_match_ptr(virtual_imx6ul_pinctrl_of_match),
        },
    .probe = virtual_imx6ul_pinctrl_probe,
};

module_platform_driver(virtual_imx6ul_pinctrl_driver);
MODULE_LICENSE("GPL");