#include "ControlConfig.h"
#include <QDataStream>


QDataStream& operator>>(QDataStream& str, ControlType& ct) {
  unsigned int type = 0;
  str >> type;
  ct = static_cast<ControlType>(type);
  return str;
}



QDataStream& operator>>( QDataStream& d, ControlConfig& ct ) {
    d >> ct.type >>ct.params;
    return d;
}

