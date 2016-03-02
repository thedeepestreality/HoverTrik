#include "hovertrik.h"
#include <iostream>

using namespace std;

HoverTrik::HoverTrik(QObject *parent) :
    QObject(parent)
{
    brick = trikControl::BrickFactory::create(".", ".");
    fsr = 10;
    dt = 1/fsr;
    ms = 0;
    state = 0;

    //Open I2C file
//    int file = open("/dev/i2c-1",O_RDWR);
//    if (file < 0)
//        printf("File error!\n");
//    else
//        printf("File opened!\n");

//    if (ioctl(file, I2C_SLAVE, 0x68)<0)
//    {
//        printf("ioctl error!\n");

//    }
//    else
//        printf("ioctl ok!\n");

//    set_file(file);
   // printf("open: %d\n",mympu_open(fsr));

    //Init motor variables
    //cushion = brick->motor("E1");
    //thrust = brick->motor("E2");


    //ПРОБЛЕМА С СЕРВОЙ
    rudder = brick->motor("S4");
    //НУЛЕВОЙ УКАЗАТЕЛЬ
    cout << rudder << endl;
    //SEGMENTATION FAULT
    rudder->setPower(0);

    //Init regulators
   // cushion->setPower(-90);
    //thrust->setPower(-90);
    //rudder->setPower(0);



//    printf("initok\n");

//    imuTimer = new QTimer(this);
//    connect(imuTimer,SIGNAL(timeout()),this,SLOT(timeout()));

//    QTimer::singleShot(25000,this,SLOT(motorStart()));
//    printf("Let's go!\n");
//    state = 0;

//   // motorStart = new QTimer(this);
//    //delay(5000);
//    imuTimer->start(1000/fsr);

//    init_timer();


    //clock_gettime(CLOCK_REALTIME, &spec);
}

void HoverTrik::motorStart()
{
    switch (state)
    {
        case 0:
        {
            printf("Start cushion!\n");
            //cushion->setPower(0);
            state = 1;
            QTimer::singleShot(1000,this,SLOT(motorStart()));
            break;
        }
        case 1:
        {
            printf("Cushion to position! Thruster on!\n");
            //cushion->setPower(70);
            //thrust->setPower(40);
            state = 2;
            QTimer::singleShot(10000,this,SLOT(motorStart()));
            break;
        }
        case 2:
        {
            printf("Turn left!\n");
            rudder->setPower(-25);
            state = 3;
            QTimer::singleShot(10000,this,SLOT(motorStart()));
            break;
        }
        case 3:
        {
            printf("Turn right!\n");
            rudder->setPower(25);
            state = 4;
            QTimer::singleShot(10000,this,SLOT(motorStart()));
            break;
        }
        case 4:
        {
            printf("Stopping!\n");
            imuTimer->stop();

            //cushion->setPower(-100);
            //thrust->setPower(-100);
            rudder->setPower(0);


            QFile outFile("out.txt");
            outFile.open(QIODevice::WriteOnly);
            QTextStream ts(&outFile);
            printf("begin!\n");
            for (int i=0;i<vecSens.size();++i)
            {
                for (int j=0;j<10;++j)
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

void HoverTrik::timeout()
{

    mympu_update();

    ms = millis();
    if (ms > CALIB_OFFSET)
    {
       // mrTimer->stop();
        double* sensVals = new double[10];
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


//        if (ms > WORKING_TIME)
//        {
//            imuTimer->stop();
//            QFile outFile("out.txt");
//            outFile.open(QIODevice::WriteOnly);
//            QTextStream ts(&outFile);
//            printf("begin!\n");
//            for (int i=0;i<vecSens.size();++i)
//            {
//                for (int j=0;j<10;++j)
//                {
//                    ts << vecSens[i][j] << "\t";
//                }
//                delete[] vecSens[i];

//                ts << endl;
//            }
//            printf("done!\n");
//            outFile.close();
//        }

    }
}
