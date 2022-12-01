#ifndef _BUTTON_H
#define _BUTTON_H

#include "linux/gpio/consumer.h"
#include <linux/interrupt.h>
#include <linux/timer.h>

typedef struct btn_desc {
  int gpio;
  int flags;
  int irq;
  char *name;
  struct gpio_desc *desc;
  struct timer_list timer;
  struct tasklet_struct key_task;
} btn_desc;

#endif // !_BUTTON_H
