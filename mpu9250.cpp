#include "mpu9250.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>

MPU9250::MPU9250(QObject *parent) :
    QObject(parent)
{
    //trikControl::BrickInterface *brick = trikControl::BrickFactory::create(".", ".");
    //int res = i2c_smbus_read_word_data(0, 0);
    int file = open("/dev/i2c-1",O_RDWR);
    if (file < 0)
        printf("File error!\n");
    else
        printf("File opened!");

    if (ioctl(file, I2C_SLAVE, 0x68))
    {
        printf("ioctl error!\n");

    }
    else
        printf("ioctl ok!\n");
}
