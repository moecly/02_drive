#include "linux/export.h"
#include "linux/gfp.h"
#include "linux/kernel.h"
#include "linux/printk.h"
#include <linux/completion.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define MOD_NUM(a, b) (a % b)
#define EEPROM_SIZE 512

static struct i2c_adapter *adapter;
static char eeprom_buf[EEPROM_SIZE];
static int eeprom_idx;

static int eeprom_emulate_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs) {
  int i = 0;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  if (msgs->flags & I2C_M_RD) {
    for (; i < msgs->len; i++) {
      msgs->buf[i] = eeprom_buf[eeprom_idx++];
      eeprom_idx = MOD_NUM(eeprom_idx, EEPROM_SIZE);
    }
  } else {
    if (msgs->len < 1)
      return -1;

    eeprom_idx = msgs->buf[0];
    for (i = 1; i < msgs->len; i++) {
      eeprom_buf[eeprom_idx++] = msgs->buf[i];
      eeprom_idx = MOD_NUM(eeprom_idx, EEPROM_SIZE);
    }
  }
  return 0;
}

static int i2c_bus_virtual_master_xfer(struct i2c_adapter *adap,
                                       struct i2c_msg *msgs, int num) {
  int i = 0;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  for (; i < num; i++) {
    if (msgs[i].addr == 0x50) {
      eeprom_emulate_xfer(adapter, &msgs[i]);
    } else {
      return -1;
    }
  }

  return 0;
}

static u32 i2c_bus_virtual_func(struct i2c_adapter *adap) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return I2C_FUNC_I2C | I2C_FUNC_NOSTART | I2C_FUNC_SMBUS_EMUL |
         I2C_FUNC_SMBUS_READ_BLOCK_DATA | I2C_FUNC_SMBUS_BLOCK_PROC_CALL |
         I2C_FUNC_PROTOCOL_MANGLING;
}

static const struct of_device_id i2c_bus_virtual_dt_ids[] = {
    {
        .compatible = "100ask,i2c-bus-virtual",
    },
    {/* sentinel */},
};

static struct i2c_algorithm i2c_algo = {
    .master_xfer = i2c_bus_virtual_master_xfer,
    .functionality = i2c_bus_virtual_func,
};

static int i2c_probe(struct platform_device *pdev) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  adapter = devm_kzalloc(&pdev->dev, sizeof(struct i2c_adapter), GFP_KERNEL);

  if (!adapter)
    goto err_kzalloc;

  adapter->owner = THIS_MODULE;
  adapter->algo = &i2c_algo;
  adapter->nr = -1;
  adapter->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
  snprintf(adapter->name, sizeof(adapter->name), "i2c-bus-virtual");

  i2c_add_adapter(adapter);

  return 0;
err_kzalloc:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return -1;
}

static int i2c_remove(struct platform_device *pdev) {
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  i2c_del_adapter(adapter);
  return 0;
}

static struct platform_driver i2c_drv = {
    .driver =
        {
            .name = "i2c-adapter",
            .of_match_table = i2c_bus_virtual_dt_ids,
        },
    .probe = i2c_probe,
    .remove = i2c_remove,
};

module_platform_driver(i2c_drv);

MODULE_LICENSE("GPL");
