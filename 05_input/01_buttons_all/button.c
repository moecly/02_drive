#include "button.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/of.h"
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

static btn_desc *btn;
static int btn_buf[BUF_LEN];
static int push_idx = 0;
static int pop_idx = 0;
static int major = 0;
static struct class *btn_class;
static DECLARE_WAIT_QUEUE_HEAD(btn_wait);

#define HAVE_VAL() (push_idx != pop_idx)

/*
 * ring buffer put val.
 */
static void put_val(int val) {
  btn_buf[push_idx] = val;
  push_idx = NEXT_POS(push_idx);
}

/*
 * ring buffer get val.
 */
static int get_val(void) {
  int ret;

  if (!HAVE_VAL()) {
    return -1;
  }

  ret = btn_buf[pop_idx];
  pop_idx = NEXT_POS(pop_idx);
  return ret;
}

/*
 * key software anti-shake.
 */
static void btn_timer_func(unsigned long data) {
  btn_desc *bdesc = (btn_desc *)data;
  int gpio;
  int val;

  val = gpiod_get_value(bdesc->desc);
  gpio = (bdesc->gpio << 8 | val);
  put_val(gpio);
  wake_up_interruptible(&btn_wait);
}

static irqreturn_t btn_irq_func(int irq, void *data) {
  btn_desc *bdesc = (btn_desc *)data;
  mod_timer(&bdesc->timer, jiffies + HZ / 50);
  return IRQ_WAKE_THREAD;
}

static irqreturn_t btn_irq_thread_func(int irq, void *data) {
  return IRQ_HANDLED;
}

static ssize_t btn_read(struct file *file, char __user *buf, size_t size,
                        loff_t *offset) {
  int val;
  int ret;

  if (!HAVE_VAL() && (file->f_flags & O_NONBLOCK)) {
    return -EAGAIN;
  }

  wait_event_interruptible(btn_wait, HAVE_VAL());
  printk("test\n");
  val = get_val();
  ret = copy_to_user(buf, &val, 4);
  return 4;
}

static unsigned int btn_poll(struct file *file,
                             struct poll_table_struct *wait) {
  poll_wait(file, &btn_wait, wait);
  return HAVE_VAL() ? POLL_IN | POLLRDNORM : 0;
}

static int btn_fasync(int fd, struct file *file, int on) { return 0; }

static struct file_operations btn_fops = {
    .read = btn_read,
    .poll = btn_poll,
    .fasync = btn_fasync,
};

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
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  count = of_gpio_count(node);
  if (count < 0) {
    printk("get gpio count err\n");
    goto err1;
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
      goto err1;
    }
  }

  /*
   * register major.
   */
  major = register_chrdev(0, "btn", &btn_fops);
  if (major < 0) {
    printk("register chrdev err\n");
    goto err1;
  }

  /*
   * register class.
   */
  btn_class = class_create(THIS_MODULE, "btn");
  if (IS_ERR(btn_class)) {
    ret = PTR_ERR(btn_class);
    printk("class create err\n");
    goto err0;
  }

  /*
   * register device.
   */
  device_create(btn_class, NULL, MKDEV(major, 0), NULL, "btn");
  return 0;

err0:
  unregister_chrdev(major, "btn");
err1:
  return -1;
}

/*
 * button exit.
 */
static int btn_remove(struct platform_device *pdev) {
  int count;
  int i = 0;
  struct device_node *node = pdev->dev.of_node;

  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  /*
   * get button count.
   */
  count = of_gpio_count(node);
  for (; i < count; i++) {
    free_irq(btn[i].irq, &btn[i]);
    del_timer(&btn[i].timer);
  }

  device_destroy(btn_class, MKDEV(major, 0));
  class_destroy(btn_class);
  unregister_chrdev(major, "btn");
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