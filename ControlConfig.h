#ifndef CONTROLCONFIG_H
#define CONTROLCONFIG_H
#include <QList>

enum ControlType {manual, cnst, pid, data};

class ControlConfig
{
public:
    ControlConfig(ControlType _type = manual)
    {
        type = _type;
    }

    ControlType getType() const
    {
        return type;
    }
    QList<double> getParams() const
    {
        return params;
    }
    void append(double val)
    {
        params.append(val);
    }

    friend QDataStream& operator>>( QDataStream& d, ControlConfig& ct );

private:
    ControlType type;
    QList<double> params;
};

#endif // CONTROLCONFIG_H
