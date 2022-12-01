#include "linux/ioport.h"
#include "linux/mod_devicetable.h"
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio_keys.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sysctl.h>
#include <linux/workqueue.h>

static int ts_probe(struct platform_device *pdev) {
  int ret;
  struct input_dev *idev = devm_input_allocate_device(&pdev->dev);
  struct resource *io;

  if (idev)
    goto err_devm_input_allocate_device;

  /*
   * set touchscreen params.
   */
  platform_set_drvdata(pdev, idev);
  __set_bit(EV_ABS, idev->evbit);
  __set_bit(EV_KEY, idev->evbit);
  __set_bit(BTN_TOUCH, idev->keybit);
  __set_bit(ABS_X, idev->absbit);
  __set_bit(ABS_Y, idev->absbit);
  input_set_abs_params(idev, ABS_X, 0, 0xffff, 0, 0);

  /*
   * register input drive.
   */
  ret = input_register_device(idev);
  if (ret)
    goto err_input_register_device;

  /*
   * get resource.
   */
  io = platform_get_resource(pdev, IORESOURCE_MEM, 0);

  return 0;

err_input_register_device:
  input_free_device(idev);
err_devm_input_allocate_device:
  return -1;
}

static int ts_remove(struct platform_device *pdev) {
  struct input_dev *idev = platform_get_drvdata(pdev);
  input_unregister_device(idev);
  input_free_device(idev);
  return 0;
}

static struct platform_driver ts_dev = {
    .probe = ts_probe,
    .remove = ts_remove,
};

module_platform_driver(ts_dev);
MODULE_LICENSE("GPL");