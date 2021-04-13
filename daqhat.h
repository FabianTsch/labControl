#ifndef DAQHAT_H
#define DAQHAT_H

// Qt
#include <QObject>

// external Libs
#include<daqhats/daqhats.h>

class daqhat : public QObject
{
    Q_OBJECT
public:
    explicit daqhat(QObject *parent = nullptr);

signals:

};

#endif // DAQHAT_H
