#ifndef ENCODER_H
#define ENCODER_H

#define CHA 6
#define CHB 3
#define ANGLE_STEP 1.44

//Qt
#include <QObject>
#include <QtCore>
#include <QVector>
#include <QThread>

// STD
#include <cmath>
#include <wiringPi.h>

class Encoder : public QObject{
    Q_OBJECT
private:
    int chA_past;
    int chB_past;
    const size_t readingRounds = 125;

public slots:
    void measure();

signals:
    void resultReady(const QVector<double> &resultAgl);

public:

};




#endif
