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
#include <pigpiod_if2.h>

#define uint32_t u_int32_t

class Encoder : public QObject{
    Q_OBJECT
private:
    int pi;
    unsigned int gpioA;
    unsigned int gpioB;
    // place function pointer here
    int cb_id_a;
    int cb_id_b;
    int levA;
    int levB;
    int oldState;
    int glitch;
    int mode;
    int step;

    // old Version
    int chA_past;
    int chB_past;
    const size_t readingRounds = 125;

public:
    Encoder(int pi, int gpioA, int gpioB);
    ~Encoder();
    friend void _cb(int pi, unsigned gpio, unsigned level,
                    uint32_t tick, void *user);
    friend void _cb_singel_rise(int pi, unsigned gpio,
                                unsigned level, uint32_t tick,
                                void *user);

public slots:
    void getResults();
    void measure();

signals:
    void resultReady(int results);

};




#endif
