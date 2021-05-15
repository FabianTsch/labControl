#ifndef ATMEGA328P_H
#define ATMEGA328P_H

#include <QObject>
#include <QtCore>
#include <wiringPiI2C.h>

#define DEVICE_ID 0x05

class EncoderControl : public QObject
{
  Q_OBJECT
private:
    int fd;
    int32_t a1,b1,c1,d1;
    int32_t received_data_encoder;
    float received_data_loadCell1;
    float received_data_loadCell2;


public:
    EncoderControl(QObject *parent = nullptr);
public slots:
    void getResults();
    void clear();

signals:
    void resultReady(int results_encoder, float results_loadCell1, float results_LoadCell2);
};

#endif // ATMEGA328P_H
