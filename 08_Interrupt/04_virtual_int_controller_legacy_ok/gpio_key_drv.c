#include <linux/module.h>

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/tty.h>

struct gpio_key {
  char name[100];
  int irq;
  int cnt;
};

static struct gpio_key gpio_keys_100ask[100];

static irqreturn_t gpio_key_isr(int irq, void *dev_id) {
  struct gpio_key *gpio_key = dev_id;
  printk("gpio_key_isr %s cnt %d\n", gpio_key->name, gpio_key->cnt++);
  return IRQ_HANDLED;
}

/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int gpio_key_probe(struct platform_device *pdev) {
  int err;
  int i = 0;
  int irq;

  printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
  while (1) {
    irq = platform_get_irq(pdev, i);
    if (irq <= 0)
      break;
    gpio_keys_100ask[i].irq = irq;
    sprintf(gpio_keys_100ask[i].name, "100as_virtual_key%d", i);

    err = devm_request_irq(&pdev->dev, gpio_keys_100ask[i].irq, gpio_key_isr,
                           IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                           gpio_keys_100ask[i].name, &gpio_keys_100ask[i]);
    printk("devm_request_irq %d for %s, err = %d\n", irq,
           gpio_keys_100ask[i].name, err);
    i++;
  }
  return 0;
}

static const struct of_device_id ask100_keys[] = {
    {.compatible = "100ask,gpio_key"},
    {},
};

/* 1. 定义platform_driver */
static struct platform_driver gpio_keys_driver = {
    .probe = gpio_key_probe,
    .driver =
        {
            .name = "100ask_gpio_key",
            .of_match_table = ask100_keys,
        },
};

module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_LICENSE("GPL");
