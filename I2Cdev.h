#ifndef I2CDEV_H
#define I2CDEV_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <cstdio>

#define delay_ms delay
#define uint8_t __u8
#define int8_t char


void set_file(uint8_t file);
int8_t i2c_read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buf);
int8_t i2c_write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buf);
void delay(int ms);

long millis();



#endif // I2CDEV_H
