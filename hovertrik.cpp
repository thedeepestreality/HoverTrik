#include "hovertrik.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <QDebug>
#include <QString>

using namespace std;

HoverTrik::HoverTrik(HoverConfig _config, QObject *parent) :
    QObject(parent)
{
    //brick->keys()->

    config = _config;
    //config.fsr = 20;

    cushStab = false;
    brick = trikControl::BrickFactory::create(".", ".");
    connect(brick->keys(),SIGNAL(buttonPressed(int,int)),this,SLOT(buttonPressed(int,int)));
    printf("Hello!\r\n");
    brick->display()->showImage("/media/trik_smile_normal.png");
    brick->display()->redraw();

    TcpServ = new QTcpServer(this);
    if (!TcpServ->listen(QHostAddress("192.168.77.1"),2016))
    {
        brick->display()->clear();
        brick->display()->addLabel("TcpServer failed to listen",50,50);
        brick->display()->redraw();
        printf("TcpServer error\r\n");
        this->thread()->quit();
    }

    connect(TcpServ,SIGNAL(newConnection()),this,SLOT(clientConnected()));
    TcpSocket = new QTcpSocket(this);
    //brick->display()->setBackground("");
    //Data rate (Hz)

    fsr = config.fsr;//50;
    //Period in ms
    dt = 1.0/fsr;
    printf("dt = %f\r\n",dt);
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
    printf("berofr\n\r");
    init_timer();

//    //Init motor variables
    cushionL = brick->motor("S1");
    cushionR = brick->motor("S3");
    motorL = brick->motor("S5");
    motorR = brick->motor("S2");

//    //Init regulators
    cushionL->setPower(-80);
    cushionR->setPower(-80);
    motorL->setPower(-80);
    motorR->setPower(25);


    //If we got here then initialization worked well
    printf("Init OK\n");
    printf("TIMER: %d\n",1000/fsr);

//    //Wait for IMU to calibrate, then let's go
//    QTimer::singleShot(conf.imuDuration*1000,this,SLOT(motorStart()));
//    printf("Let's go!\n");
//    state = 0;
//    ang = 0;
//    leftCush = -80;
//    rightCush = -80;
//    cushionTimer = new QTimer(this);
//    connect(cushionTimer,SIGNAL(timeout()),this,SLOT(pushCush()));

//    stateTimer = new QTimer(this);
//    connect(stateTimer,SIGNAL(timeout()),this,SLOT(printState()));
//    stateTimer->start(1000);
//    printf("Passed\r\n");

}

void HoverTrik::clientConnected()
{
    printf("new client!\r\n");
    TcpSocket = TcpServ->nextPendingConnection();
    connect(TcpSocket,SIGNAL(readyRead()),this,SLOT(socketNewData()));
    DS.setDevice(TcpSocket);

}

int HoverTrik::saturation(int val, int lvl)
{
    int retVal = val > lvl ? lvl : val;
    retVal = retVal < -lvl ? -lvl : val;

    return retVal;
}

void HoverTrik::socketNewData()
{
    //printf("new data!\r\n");
    //while(TcpSocket.)
    while(TcpSocket->bytesAvailable() > 0)
    {
        DS >> conf;
        switch(conf.getType())
        {
            case ControlType::manual:
            {
                int thrust = saturation(conf.getParams()[0],80);
                int rudder = saturation(conf.getParams()[1],80);
                motorL->setPower(thrust);
                motorR->setPower(rudder);
                //printf("%d %d\r\n",thrust,rudder);
                break;
            }
            case ControlType::cnst:
            {
                int cushL = saturation(conf.getParams()[0],80);
                int cushR = saturation(conf.getParams()[1],80);
                cushionL->setPower(cushL);
                cushionR->setPower(cushR);

                break;
            }
        }
    }
//    while (TcpSocket->canReadLine())
//    {
//        QString str = TcpSocket->readLine();
//        qDebug() << str;
//        QStringList args = str.split(' ');
//        if (!args[0].compare("conf"))
//        {
//            qDebug() << "Config arrived!";
//        }
//    }
    //TcpSocket->read(data,available);
    //TcpSocket->write(data,available);
}

void HoverTrik::buttonPressed(int keyCode, int value)
{
    printf("Button pressed: %d %d\r\n");

    if (keyCode == 1 && value == 1)
        this->thread()->quit();


   // else if (value == 1)
   // {
        brick->display()->clear();
        brick->display()->addLabel("Button: "+QString::number(keyCode),50,50);
        brick->display()->redraw();
    //}

}

void HoverTrik::printState()
{

    printf("Time passed: %d\r\n",++msPassed);

}

void HoverTrik::pushCush()
{
    leftCush+=5;
    rightCush+=5;

    cushionL->setPower(leftCush);
    cushionR->setPower(rightCush);
    if (leftCush >= config.leftC)
    {
        cushionTimer->stop();
        state = 2;

        QTimer::singleShot(2000,this,SLOT(motorStart()));
    }
}

//Motor control
void HoverTrik::motorStart()
{
    switch (state)
    {
        case 0:
        {
            printf("Start cushion!\n");
            stateTimer->stop();
            //cushionL->setPower(0);
            //cushionR->setPower(50);
//            //cushion->setPower(0);
//            state = 1;
//            QTimer::singleShot(2000,this,SLOT(motorStart()));
            cushionTimer->start(100);
            break;
        }
        case 1:
        {
            printf("Cushion to position!\n");
            cushionL->setPower(-40);
            cushionR->setPower(60);
            //cushion->setPower(80);
            //thrust->setPower(0);
            //rudder->setPower(2*(11-mympu.ypr[0]));
            state = 5;
            QTimer::singleShot(10000,this,SLOT(motorStart()));
            break;
        }
        case 2:
        {
            ang = mympu.ypr[0];
            printf("Thruster on!\n");
           // motorL->setPower(-60);
           // motorR->setPower(-50);
            cushStab = true;
            motorL->setPower(config.leftM);
            //motorR->setPower(config.rightM);

            //cushionL->setPower(0);
            //cushionL->setPower(60);

            //thrust->setPower(30);
            //rudder->setPower(3);
            //rudder->setPower(-60);
            state = 5;
            QTimer::singleShot(config.duration*1000,this,SLOT(motorStart()));
            break;
        }
        case 3:
        {
            printf("Turn left!\n");
            //rudder->setPower(-10);
            state = 4;
            QTimer::singleShot(15000,this,SLOT(motorStart()));
            break;
        }
        case 4:
        {
            printf("Turn right!\n");
            //rudder->setPower(10);
            state = 5;
            QTimer::singleShot(5000,this,SLOT(motorStart()));
            break;
        }
        case 5:
        {
            printf("Stopping!\n");
            imuTimer->stop();

            cushionL->setPower(-80);
            cushionR->setPower(-80);
            motorL->setPower(-80);
            motorR->setPower(25);
//            cushion->setPower(-90);
//            thrust->setPower(-90);
//            rudder->setPower(0);


            ofstream outFile("out.txt");
            //outFile.open(QIODevice::WriteOnly);
            //QTextStream ts(&outFile);
            printf("begin!\n");
            for (int i=0;i<vecSens.size();++i)
            {
                for (int j=0;j<16;++j)
                {
                    outFile << vecSens[i][j] << "\t";
                }
                delete[] vecSens[i];

                outFile << endl;
            }
            printf("done!\n");
            outFile.close();
            //qApp->exit();
            //this->deleteLater();
            this->thread()->quit();

            //emit finish();
            break;
        }
    }
}

QDataStream& operator<<( QDataStream& d, const ControlConfig& ct ) {
    d << ct.getType() << ct.getParams();
    return d;
}

//Data measurements
void HoverTrik::timeout()
{
    //static double integ= 0;
    mympu_update();
   // qDebug() << mympu.magneto[0] << "\t"<< mympu.magneto[1] << "\t" << mympu.magneto[2];
   //qDebug() << mympu.ypr[0];

    ms = millis();
    ControlConfig CC(data);
//    if (ms > CALIB_OFFSET*1000)
//    {
//       // mrTimer->stop();
       double* sensVals = new double[17];
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
        sensVals[16] = 0;
        if (TcpSocket->state() == QAbstractSocket::ConnectedState)
        {
            //cout << "Sending" << endl;
            for (int i=0;i<17;++i)
               CC.append(sensVals[i]);
            QDataStream ds(TcpSocket);
            ds << CC;

        }

//        if (cushStab)
//        {
//            double diff = ang - sensVals[0] + config.lvl;
//            integ += diff*1.0/config.fsr;
//            double phi_p = config.phi_p;
//            double phi_i = config.phi_i;
//            double w_p = config.w_p;

//            double pid=config.rightM + phi_p*(diff) + phi_i*integ + w_p*sensVals[3];//+integ);
//            pid = pid > 80 ? 80 : pid;
//            pid = pid < -80 ? -80 : pid;

//            motorR->setPower(pid);
//            sensVals[16] = pid;
//            printf("Diff: %f %f %f %f\r\n",sensVals[0],ang,diff,pid);
//        }
//        vecSens.push_back(sensVals);
           delete[] sensVals;

////        double contr = 25 - 0.4*(mympu.ypr[4]);
////        contr = contr > 80 ? 80 : contr;
////        contr = contr < -80 ? -80 : contr;
////        motorR->setPower(contr);

//        //if (ms < CALIB_OFFSET*1000 + 20000)
//        //   rudder->setPower(1*(ang-mympu.ypr[0]));
//        //double rud = 5*std::sin((double)ms*3.1416/3000);
//        //rudder->setPower(rud);
//    }
}
