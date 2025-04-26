#ifndef SYSTEMERROR_H
#define SYSTEMERROR_H
#include <QString>
#include <QMap>
#include "QObject"
#include "exception.h"

class SystemError : public QObject{
    Q_OBJECT

public:
    SystemError();
    void errorReport(Exception ex);

signals:
    void reported(Exception ex);

public:
    static SystemError *instance();
};

#endif // SYSTEMERROR_H
