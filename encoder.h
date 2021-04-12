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
    const size_t readingRounds = 250;

public slots:
    void measure();

signals:
    void resultReady(const QVector<double> &resultAgl);

public:

  //Encoder(std::vector<double>*, std::vector<double>*, std::vector<double>*);
  //void operator()(int, bool&);      // Eigener Thread für das lesen der Messdaten
  //double measure();
  //void addCalcValue(double&);
  //void clean();

  //double getCurrentAngle();
  //double getCurrentSpeed();
  //double getCurrentAcc();

  //std::vector<double>* getAngleVector();
  //std::vector<double>* getSpeedVector();
  //std::vector<double>* getAccVector();


};




#endif
