#include "hovertrik.h"
#include <iostream>
#include <cmath>

using namespace std;

HoverTrik::HoverTrik(QObject *parent) :
    QObject(parent)
{
    brick = trikControl::BrickFactory::create(".", ".");
    //Data rate (Hz)
    fsr = 50;
    //Period in ms
    dt = 1/fsr;
    //Current time in ms
    ms = 0;
    //Control state
    state = 0;

    //CONFIGURING MPU-9250
    //Open I2C file
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
    //Power up MPU-9250
    printf("I2C open: %d\n",mympu_open(fsr));

    //Timer for data measurement
    imuTimer = new QTimer(this);
    connect(imuTimer,SIGNAL(timeout()),this,SLOT(timeout()));
    imuTimer->start(1000/fsr);

    //Init time measurement stuff
    init_timer();

    //Init motor variables
    cushion = brick->motor("S1");
    thrust = brick->motor("S5");
    rudder = brick->motor("S3");

    //Init regulators
    cushion->setPower(-90);
    thrust->setPower(-90);
    rudder->setPower(0);


    //If we got here then initialization worked well
    printf("Init OK\n");
    printf("TIMER: %d\n",1000/fsr);

    //Wait for IMU to calibrate, then let's go
    QTimer::singleShot(25000,this,SLOT(motorStart()));
    printf("Let's go!\n");
    state = 0;
    ang = 0;

}

//Motor control
void HoverTrik::motorStart()
{
    switch (state)
    {
        case 0:
        {
            printf("Start cushion!\n");
            cushion->setPower(0);
            state = 1;
            QTimer::singleShot(2000,this,SLOT(motorStart()));
            break;
        }
        case 1:
        {
            printf("Cushion to position!\n");
            cushion->setPower(80);
            //thrust->setPower(0);
            //rudder->setPower(2*(11-mympu.ypr[0]));
            state = 2;
            QTimer::singleShot(4000,this,SLOT(motorStart()));
            break;
        }
        case 2:
        {
            ang = mympu.ypr[0];
            printf("Thruster on!\n");
            thrust->setPower(30);
            //rudder->setPower(3);
            //rudder->setPower(-60);
            state = 5;
            QTimer::singleShot(10000,this,SLOT(motorStart()));
            break;
        }
        case 3:
        {
            printf("Turn left!\n");
            rudder->setPower(-10);
            state = 4;
            QTimer::singleShot(15000,this,SLOT(motorStart()));
            break;
        }
        case 4:
        {
            printf("Turn right!\n");
            rudder->setPower(10);
            state = 5;
            QTimer::singleShot(5000,this,SLOT(motorStart()));
            break;
        }
        case 5:
        {
            printf("Stopping!\n");
            imuTimer->stop();

            cushion->setPower(-90);
            thrust->setPower(-90);
            rudder->setPower(0);


            QFile outFile("out.txt");
            outFile.open(QIODevice::WriteOnly);
            QTextStream ts(&outFile);
            printf("begin!\n");
            for (int i=0;i<vecSens.size();++i)
            {
                for (int j=0;j<16;++j)
                {
                    ts << vecSens[i][j] << "\t";
                }
                delete[] vecSens[i];

                ts << endl;
            }
            printf("done!\n");
            //qApp->exit();
            //this->deleteLater();
            this->thread()->quit();
            outFile.close();
            emit finish();
            break;
        }
    }
}

//Data measurements
void HoverTrik::timeout()
{

    mympu_update();

    ms = millis();
    if (ms > CALIB_OFFSET*1000)
    {
       // mrTimer->stop();
        double* sensVals = new double[16];
        QVector<int> gyro = brick->gyroscope()->read();
        QVector<int> accel = brick->accelerometer()->read();
        sensVals[0] = mympu.ypr[0];
        sensVals[1] = mympu.ypr[1];
        sensVals[2] = mympu.ypr[2];
        sensVals[3] = mympu.gyro[0];
        sensVals[4] = mympu.gyro[1];
        sensVals[5] = mympu.gyro[2];
        sensVals[6] = mympu.accel[0];
        sensVals[7] = mympu.accel[1];
        sensVals[8] = mympu.accel[2];
        sensVals[9] = gyro[0];
        sensVals[10] = gyro[1];
        sensVals[11] = gyro[2];
        sensVals[12] = accel[0];
        sensVals[13] = accel[1];
        sensVals[14] = accel[2];

        sensVals[15] = millis();
        vecSens.push_back(sensVals);
        //if (ms < CALIB_OFFSET*1000 + 20000)
        //   rudder->setPower(1*(ang-mympu.ypr[0]));
        //double rud = 5*std::sin((double)ms*3.1416/3000);
        //rudder->setPower(rud);
    }
}
