#ifndef BTS7960
#define BTS7960

// Qt
#include <QObject>
#include <QtCore>
#include <QVector>
#include <QThread>
#include <pigpiod_if2.h>


// LIB and STD
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <vector>

class Bts7960_sPWM : public QObject{
    Q_OBJECT
private:
    int pi;
    const int hFrequency = 100;
    const int sFrequency = 800;
    bool hardwareSupportPwm;
    bool running;
    char direction;
    bool inertia = false;
    double dblInertia;
    int R_is;
    int R_en;
    int R_pwm;
    int L_is;
    int L_en;
    int L_pwm;
    double dutyCycle = 0;

public:
    Bts7960_sPWM(int addr,int* pin = nullptr, QObject* parent = nullptr);
    void setDirection(const char d);
    void enabelInertiaContoller(const bool &);
    void setDutyCycle(const double&);
    void setInertiaLoad(const double&);
    bool checkInertiaEnabled();
    bool checkRunning();


public slots:
    void startPressed();
    void applyPressed();
    void stopPressed();

signals:

};


#endif //_BTS7960
