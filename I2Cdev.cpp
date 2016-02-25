#include "I2Cdev.h"

uint8_t i2c_file;

void set_file(uint8_t file)
{
    i2c_file = file;
}

int8_t i2c_read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buf)
{
    //__u8 buf[14] = {0};

//    if (length > 32)
//    {
//        length = 32;
//    }

//    struct i2c_smbus_ioctl_data args;
//    args.read_write= I2C_SMBUS_READ;
//    args.command = regAddr;
//    //args.size = length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA;
//    args.size = I2C_SMBUS_I2C_BLOCK_DATA;
//    union i2c_smbus_data data;
//    data.block[0] = length;
//    args.data = &data;

//    if (length == 2)
//    {
//        args.size = I2C_SMBUS_WORD_DATA;
//        ioctl(i2c_file,I2C_SMBUS,&args);
//        printf("READWORDATA: %d\n",data.word);
//        return 1;
//    }

//    if (ioctl(i2c_file,I2C_SMBUS,&args))
//    {
//        return !-1;
//    }
//    else
//    {
//        printf("READ:\n");
//        printf("%d\n",data.block[0]);
//        printf("%d\n",data.block[1]);
//        for (int i=1;i <= data.block[0];++i)
//        {
//            buf[i-1] = data.block[i];
//            printf("%d %d\n",i,data.block[i]);
//        }
//        printf("%d\nEND READ\n",data.block[length+1]);

//        return !data.block[0];
//    }

    unsigned char addr[1] = {regAddr};
    write(i2c_file,addr,1);
    read(i2c_file,buf,length);
    return 0;
}

int8_t i2c_write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *buf)
{
    unsigned char pack[length+1];
    pack[0] = regAddr;
    for (int i=0;i<length;++i)
        pack[i+1] = buf[i];
    //printf("WRITTEN: %d\n",write(i2c_file,pack,length+1));
    write(i2c_file,pack,length+1);
    return 0;
//    //__u8 buf[14] = {0};

////    if (length > 32)
////    {
////        length = 32;
////    }

//    struct i2c_smbus_ioctl_data args;
//    args.read_write= I2C_SMBUS_WRITE;
//    args.command = regAddr;

//    union i2c_smbus_data data;

//    if (length == 1)
//    {
//        args.size = I2C_SMBUS_BYTE_DATA;
//        data.byte = buf[0];
//    }
//    else if (length == 2)
//    {
//        args.size = I2C_SMBUS_WORD_DATA;
//        data.word = 0;//(unsigned short) (buf[0] << 8) | buf[1];
//        printf("word i2c: %d\n",data.word);
//    }
//    else
//    {
//        printf("length: %d\n",length);
//        if (length == 2)
//            printf("word i2c: %d\n",(buf[0]<<8) | buf[1]);
//        args.size = I2C_SMBUS_BLOCK_DATA;
//        if (length > 32)
//            length = 32;
//        for (int i=1;i<=length;++i)
//            data.block[i] = buf[i-1];
//        data.block[0] = length;
//    }
//    args.data = &data;
//    int8_t res = ioctl(i2c_file,I2C_SMBUS,&args);
//    //printf("IOCTL: %d\n",res);
//    return res;



//    int ret;
//    args.size = I2C_SMBUS_BYTE_DATA;
//    for (int i=0;i<length;++i)
//    {
//        data.byte = buf[i];
//    args.data = &data;
//    ret = !ioctl(devAddr,I2C_SMBUS,&args);
//    }

//    if (ioctl(devAddr,I2C_SMBUS,&args))
//    {
//        return -1;
//    }
//    else
//    {
//        for (int i=1;i <= data.block[0];++i)
//            buf[i-1] = data.block[i];

//        return data.block[0];
//    }

//    return -1;
}

void delay(int ms)
{
    usleep(ms*1000);
}

long millis()
{
    long ms;
    time_t s;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = round(spec.tv_nsec/1.0e6);

    return ms;
}


