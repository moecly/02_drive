#include "asm/uaccess.h"
#include "linux/device.h"
#include "linux/err.h"
#include "linux/export.h"
#include <linux/acpi.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/nvmem-provider.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static int major = 0;
static struct class *ap3216c_class;
static struct i2c_client *ap3216c_client;

static ssize_t ap3216c_read(struct file *file, char __user *buf, size_t size,
                            loff_t *off) {
  char recv[6];
  int ret;
  int val;

  if (size != 6)
    goto err_recv_size;

  /*
   * get data.
   */
  val = i2c_smbus_read_word_data(ap3216c_client, 0xA);
  recv[0] = val & 0xff;
  recv[1] = (val >> 8) & 0xff;

  val = i2c_smbus_read_word_data(ap3216c_client, 0xC);
  recv[0] = val & 0xff;
  recv[1] = (val >> 8) & 0xff;

  val = i2c_smbus_read_word_data(ap3216c_client, 0xE);
  recv[0] = val & 0xff;
  recv[1] = (val >> 8) & 0xff;

  /*
   * transform data.
   */
  ret = copy_to_user(buf, recv, size);

  return size;

err_recv_size:
  return -1;
}

static ssize_t ap3216c_write(struct file *file, const char __user *buf,
                             size_t size, loff_t *off) {
  return 0;
}

static int ap3216c_release(struct inode *node, struct file *file) { return 0; }

static int ap3216c_open(struct inode *node, struct file *file) {
  i2c_smbus_write_byte_data(ap3216c_client, 0, 0x4);
  mdelay(20);
  i2c_smbus_write_byte_data(ap3216c_client, 0, 0x3);
  mdelay(250);
  return 0;
}

static struct file_operations ap3216c_fops = {
    .owner = THIS_MODULE,
    .open = ap3216c_open,
    .release = ap3216c_release,
    .write = ap3216c_write,
    .read = ap3216c_read,
    .llseek = no_llseek,
};

static int ap3216c_probe(struct i2c_client *client,
                         const struct i2c_device_id *id) {
  ap3216c_client = client;
  major = register_chrdev(0, "ap3216c", &ap3216c_fops);
  if (major < 0)
    goto err_register_chrdev;

  ap3216c_class = class_create(THIS_MODULE, "ap3216c");
  if (IS_ERR(ap3216c_class))
    goto err_class_create;

  device_create(ap3216c_class, NULL, MKDEV(major, 0), NULL, "ap3216c");

  return 0;

err_class_create:
  unregister_chrdev(major, "ap3216c");
err_register_chrdev:
  return -1;
}

static int ap3216c_remove(struct i2c_client *client) {
  device_destroy(ap3216c_class, MKDEV(major, 0));
  class_destroy(ap3216c_class);
  unregister_chrdev(major, "ap3216c");
  return 0;
}

static const struct of_device_id of_match_ids_ap3216c[] = {
    {.compatible = "lite-on,ap3216c", .data = NULL},
    {/* END OF LIST */},
};

static const struct i2c_device_id ap3216c_ids[] = {
    {"ap3216c", (kernel_ulong_t)NULL},
    {/* END OF LIST */},
};

static struct i2c_driver i2c_ap3216c_driver = {
    .driver =
        {
            .name = "ap3216c",
            .of_match_table = of_match_ids_ap3216c,
        },
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .id_table = ap3216c_ids,
};

module_i2c_driver(i2c_ap3216c_driver);
MODULE_LICENSE("GPL");
