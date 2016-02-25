#ifndef HOVERTRIK_H
#define HOVERTRIK_H

#include <QObject>
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

using namespace std;

class HoverTrik : public QObject
{
    Q_OBJECT
    trikControl::BrickInterface* brick;
    QTimer* mrTimer;
    double ms;
    //struct timespec spec;

    vector<double*> vecSens;
public:
    explicit HoverTrik(QObject *parent = 0);

signals:

public slots:
    void timeout();
};

#endif // HOVERTRIK_H
