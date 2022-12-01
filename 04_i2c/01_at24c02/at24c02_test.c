#include "i2cbusses.h"
#include <errno.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int file;
  int ret;
  char file_name[20];
  char buf[40];
  unsigned char mem_addr = 0;
  char *str;
  unsigned char dev_addr = 0x50;
  struct timespec req;

  if (argc != 3 && argc != 4) {
    printf("Usage:\n");
    printf("write eeprom: %s <i2c_bus_number> w string\n", argv[0]);
    printf("read  eeprom: %s <i2c_bus_number> r\n", argv[0]);
    goto err_input_argc;
  }

  file = open_i2c_dev(argv[1][0] - '0', file_name, sizeof(file_name), 0);
  if (file < 0)
    goto err_open_i2c_dev;

  ret = set_slave_addr(file, dev_addr, 1);
  if (ret)
    goto err_set_slave_addr;

  if (argv[2][0] == 'w') {
    str = argv[3];
    req.tv_nsec = 20000000;
    req.tv_sec = 0;
    while (*str) {
      ret = i2c_smbus_write_byte_data(file, mem_addr++, *str);
      if (ret)
        goto err_i2c_smbus_write_byte_data;
      str++;
    }
  } else {
    ret = i2c_smbus_read_i2c_block_data(file, mem_addr, sizeof(buf), buf);
    if (ret < 0)
      goto err_i2c_smbus_read_block_data;
    buf[39] = '\0';
    printf("buf = %s\n", buf);
  }

  return 0;

err_i2c_smbus_read_block_data:
  printf("err\n");
err_i2c_smbus_write_byte_data:
err_set_slave_addr:
err_open_i2c_dev:
err_input_argc:
  return -1;
}