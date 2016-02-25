#ifndef MPU9250_H
#define MPU9250_H

#include <QObject>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

//#include <trikControl/brickFactory.h>
//#include <trikControl/brickInterface.h>
//#include <trikKernel/loggingHelper.h>

class MPU9250 : public QObject
{
    Q_OBJECT
public:
    explicit MPU9250(QObject *parent = 0);

signals:

public slots:

};

#endif // MPU9250_H
