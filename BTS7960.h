#ifndef BTS7960
#define BTS7960

// Qt
#include <QObject>
#include <QtCore>
#include <QVector>
#include <QThread>


// LIB and STD
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <vector>


class Bts7960 : QObject{
    Q_OBJECT
private:
    char direction;
    bool inertia = false;
    int R_is;
    int R_en;
    int R_pwm;
    int L_is;
    int L_en;
    int L_pwm;
    unsigned int range = 100;
    int power = 0;

public:
    Bts7960(QObject* parent = nullptr);
    void setDirection(const char &);


public slots:
    //void applyPressed();
    //void startPressed();
    //void stopPressed();

signals:

  //void setPower(int);
  //void stop();
  //void start(char);
  //void checkPin();
};

class Bts7960_sPWM : public QObject{
    Q_OBJECT
private:
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
    unsigned int range = 100;
    double dutyCycle = 0;

public:
    Bts7960_sPWM(int* pin = nullptr, QObject* parent = nullptr);
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


  //void stop();
  //void start(char);
  //void checkPin();
};


#endif //_BTS7960
