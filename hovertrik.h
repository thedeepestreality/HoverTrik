#ifndef HOVERTRIK_H
#define HOVERTRIK_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QTextStream>

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

#define CALIB_OFFSET 1
#define WORKING_TIME 10

using namespace std;

class HoverTrik : public QObject
{
    Q_OBJECT
    trikControl::BrickInterface* brick;
    QTimer* imuTimer;

    double ms;
    int fsr;
    double dt;
    //struct timespec spec;
    trikControl::MotorInterface* cushion;
    trikControl::MotorInterface* thrust;
    trikControl::MotorInterface* rudder;

    int state;

    vector<double*> vecSens;
public:
    explicit HoverTrik(QObject *parent = 0);

signals:
    void finish();
public slots:
    void timeout();
    void motorStart();
};

#endif // HOVERTRIK_H
