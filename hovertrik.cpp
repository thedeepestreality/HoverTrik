#include "hovertrik.h"

HoverTrik::HoverTrik(QObject *parent) :
    QObject(parent)
{
    brick = trikControl::BrickFactory::create(".", ".");

//    uint8_t i2c_file;
    int file = open("/dev/i2c-1",O_RDWR);
    if (file < 0)
        printf("File error!\n");
    else
        printf("File opened!\n");

    if (ioctl(file, I2C_SLAVE, 0x68)<0)
    {
        printf("ioctl error!\n");

    }
    else
        printf("ioctl ok!\n");

    set_file(file);

    printf("open: %d\n",mympu_open(10));

    mrTimer = new QTimer(this);
    connect(mrTimer,SIGNAL(timeout()),this,SLOT(timeout()));
    //delay(5000);
    mrTimer->start(100);
    init_timer();
    ms = 0;

    //clock_gettime(CLOCK_REALTIME, &spec);
}

void HoverTrik::timeout()
{
    double* sensVals = new double[10];
    mympu_update();
    //clock_gettime(CLOCK_REALTIME, &spec);
    sensVals[0] = mympu.ypr[0];
    sensVals[1] = mympu.ypr[1];
    sensVals[2] = mympu.ypr[2];
    sensVals[3] = mympu.gyro[0];
    sensVals[4] = mympu.gyro[1];
    sensVals[5] = mympu.gyro[2];
    sensVals[6] = mympu.accel[0];
    sensVals[7] = mympu.accel[1];
    sensVals[8] = mympu.accel[2];

    sensVals[9] = millis();

    vecSens.push_back(sensVals);
    ms += 0.1;
    if (ms > 1)
    {
        mrTimer->stop();
        QFile outFile("out.txt");
        outFile.open(QIODevice::WriteOnly);
        QTextStream ts(&outFile);
        printf("begin!\n");
        for (int i=0;i<vecSens.size();++i)
        {
            for (int j=0;j<10;++j)
            {
                ts << vecSens[i][j] << "\t";
                //printf("%f ",vecSens[i][j]);
            }
            delete[] vecSens[i];

            ts << endl;
        }
        printf("done!\n");
        outFile.close();

    }
   //printf("%f %f\n",ms,mympu.ypr[0]);
}
