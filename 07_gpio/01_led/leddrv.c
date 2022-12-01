#include "asm/uaccess.h"
#include "linux/err.h"
#include "linux/kdev_t.h"
#include "linux/printk.h"
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/tty.h>

static int major;
static struct class *leds_class;
static struct gpio_desc *gdesc;

static ssize_t led_read(struct file *file, char __user *buf, size_t size,
                        loff_t *ppos) {
  return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t size,
                         loff_t *ppos) {
  int ret;
  char status;

  ret = copy_from_user(&status, buf, 1);
  gpiod_set_value(gdesc, status);

  return 1;
}

static int led_open(struct inode *node, struct file *file) {
  gpiod_direction_output(gdesc, 0);
  return 0;
}

static int led_release(struct inode *node, struct file *file) { return 0; }

static struct file_operations led_fops = {
    .write = led_write,
    .read = led_read,
    .open = led_open,
    .release = led_release,
    .llseek = no_llseek,
};

static int led_drv_probe(struct platform_device *pdev) {

  /*
   * get gpio desc and set platform data.
   */
  gdesc = devm_gpiod_get_index(&pdev->dev, "leds", 0, GPIOD_OUT_LOW);
  if (IS_ERR(gdesc))
    goto err_gpiod_get_index;

  // platform_set_drvdata(pdev, gdesc);

  /*
   * register led.
   */
  major = register_chrdev(0, "led", &led_fops);
  if (major < 0)
    goto err_register_chrdev;

  leds_class = class_create(THIS_MODULE, "led");
  if (IS_ERR(leds_class))
    goto err_class_create;

  device_create(leds_class, NULL, MKDEV(major, 0), NULL, "led");

  return 0;

err_class_create:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  unregister_chrdev(major, "led");
err_register_chrdev:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
err_gpiod_get_index:
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return -1;
}

static int led_drv_remove(struct platform_device *pdev) {
  // struct gpio_desc *gdesc = platform_get_drvdata(pdev);
  // gpiod_put(gdesc);
  device_destroy(leds_class, MKDEV(major, 0));
  class_destroy(leds_class);
  unregister_chrdev(major, "led");
  return 0;
}

static const struct of_device_id led_device_ids[] = {
    {
        .compatible = "imx-gpio-leds",
    },
    {},
};

static struct platform_driver led_drv = {
    .driver =
        {
            .name = "led",
            .of_match_table = led_device_ids,
        },
    .probe = led_drv_probe,
    .remove = led_drv_remove,
};

module_platform_driver(led_drv);
MODULE_LICENSE("GPL");
