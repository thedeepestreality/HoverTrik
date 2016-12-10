#ifndef HOVERTRIK_H
#define HOVERTRIK_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QVector>

#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QDataStream>
#include "ControlConfig.h"

#include <unistd.h>
#include <time.h>
#include <math.h>
#include <cstdio>
//#include <QVector>

//TRIK headers
#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>

//MPU-9250 headers
#include "mpu.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#define CALIB_OFFSET 20
#define WORKING_TIME 10



using namespace std;

struct HoverConfig{
    int leftC;
    int rightC;
    int leftM;
    int rightM;
    int fsr;
    int duration;
    int imuDuration;
    double phi_p;
    double phi_i;
    double w_p;
    double w_i;
    double lvl;
};

class HoverTrik : public QObject
{
    Q_OBJECT
    trikControl::BrickInterface* brick;
    QTimer* imuTimer;
    QTimer* cushionTimer;
    QTimer* stateTimer;
    int msPassed = 0;
    double ms;
    int fsr;
    double dt;
    //struct timespec spec;
    trikControl::MotorInterface* cushionL;
    trikControl::MotorInterface* cushionR;

    trikControl::MotorInterface* motorL;
    trikControl::MotorInterface* motorR;
    int leftCush, rightCush;
    HoverConfig config;
    bool cushStab;

    QTcpServer* TcpServ;
    QTcpSocket* TcpSocket;
    ControlConfig conf;
    QDataStream DS;
    //trikControl::MotorInterface* rudder;
    int state;
    double ang;
    vector<double*> vecSens;
private:
    int saturation(int val, int lvl);

public:
    explicit HoverTrik(HoverConfig, QObject *parent = 0);

signals:
    void finish();
public slots:
    void timeout();
    void motorStart();
    void pushCush();
    void printState();
    void buttonPressed(int,int);
    void clientConnected();
    void socketNewData();
};

#endif // HOVERTRIK_H
