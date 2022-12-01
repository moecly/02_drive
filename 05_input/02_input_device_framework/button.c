#include <linux/input.h>
#include <asm/current.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
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
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/timer.h>
#include <linux/tty.h>
#include <linux/workqueue.h>

#define BUF_LEN 200
#define NEXT_POS(x) (x + 1) % BUF_LEN

typedef struct btn_desc {
  int gpio;
  int flags;
  int irq;
  char *name;
  struct gpio_desc *desc;
  struct timer_list timer;
  struct tasklet_struct key_task;
  struct input_dev *idev;
} btn_desc;

static btn_desc *btn;
static DECLARE_WAIT_QUEUE_HEAD(btn_wait);

#define HAVE_VAL() (push_idx != pop_idx)

/*
 * key software anti-shake.
 */
static void btn_timer_func(unsigned long data) {
  btn_desc *bdesc = (btn_desc *)data;
  int gpio;
  int val;

  gpio = bdesc->gpio;
  val = gpiod_get_value(bdesc->desc);
  if (val) {
    input_event(bdesc->idev, EV_KEY, KEY_S, 0);
    input_sync(bdesc->idev);
  } else {
    input_event(bdesc->idev, EV_KEY, KEY_S, 1);
    input_sync(bdesc->idev);
  }
}

static irqreturn_t btn_irq_func(int irq, void *data) {
  btn_desc *bdesc = (btn_desc *)data;
  mod_timer(&bdesc->timer, jiffies + HZ / 50);
  return IRQ_WAKE_THREAD;
}

static irqreturn_t btn_irq_thread_func(int irq, void *data) {
  return IRQ_HANDLED;
}

/*
 * button init.
 */
static int btn_probe(struct platform_device *pdev) {
  int count;
  int i = 0;
  int ret;
  struct device_node *node = pdev->dev.of_node;
  enum of_gpio_flags flags;

  /*
   * get button count.
   */
  count = of_gpio_count(node);
  if (count < 0) {
    printk("get gpio count err\n");
    goto err;
  }

  /*
   * register button.
   */
  btn = kzalloc(count * sizeof(btn_desc), GFP_KERNEL);
  for (; i < count; i++) {
    btn[i].gpio = of_get_gpio_flags(node, i, &flags);
    btn[i].flags = flags & OF_GPIO_ACTIVE_LOW;
    btn[i].desc = gpio_to_desc(btn[i].gpio);
    btn[i].irq = gpio_to_irq(btn[i].gpio);

    btn[i].idev = input_allocate_device();
    if (!btn[i].idev) {
      printk("input allocate device err\n");
      goto err;
    }

    /*
     * repeat acquisition button.
     */
    set_bit(EV_KEY, btn[i].idev->evbit);
    set_bit(EV_REP, btn[i].idev->evbit);
    set_bit(KEY_L, btn[i].idev->keybit);
    set_bit(KEY_S, btn[i].idev->keybit);
    set_bit(KEY_ENTER, btn[i].idev->keybit);
    set_bit(KEY_LEFTSHIFT, btn[i].idev->keybit);
    ret = input_register_device(btn[i].idev);
    if (ret) {
      printk("input register device err\n");
      goto err;
    }

    /*
     * setup timer.
     */
    setup_timer(&btn[i].timer, btn_timer_func, (unsigned long)&btn[i]);
    btn[i].timer.expires = ~0;
    add_timer(&btn[i].timer);

    /*
     * setup thread.
     */
    ret = request_threaded_irq(btn[i].irq, btn_irq_func, btn_irq_thread_func,
                               IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                               "imx_btn", &btn[i]);
    if (ret) {
      printk("request thread irq err\n");
      goto err;
    }
  }

  return 0;

err:
  return -1;
}

/*
 * button exit.
 */
static int btn_remove(struct platform_device *pdev) {
  int count;
  int i = 0;
  struct device_node *node = pdev->dev.of_node;

  /*
   * get button count.
   */
  count = of_gpio_count(node);
  for (; i < count; i++) {
    free_irq(btn[i].irq, &btn[i]);
    del_timer(&btn[i].timer);
    input_unregister_device(btn[i].idev);
  }

  kfree(btn);
  return 0;
}

static const struct of_device_id btn_ids[] = {
    {.compatible = "imx,keys"},
    {},
};

static struct platform_driver btn_drv = {
    .probe = btn_probe,
    .remove = btn_remove,
    .driver =
        {
            .name = "btn",
            .of_match_table = btn_ids,
        },
};

/*
 * button drive init.
 */
static int __init button_init(void) {
  platform_driver_register(&btn_drv);
  return 0;
}

/*
 * button drive exit.
 */
static void __exit button_exit(void) { platform_driver_unregister(&btn_drv); }

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");