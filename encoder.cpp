#include "encoder.h"


void Encoder::measure(){
    QVector<double> resultAgl;
    QVector<double> resultSpd;
    QVector<double> resultAcc;
    double dblAngle = 0.0;

    for(size_t k = 0; k < 10; ++k)
    {
        // Condition Conditions to determine direction and speed
        // Channel B is leading Channel A
        int chA;
        int chB;
        for(size_t i = 0; i < readingRounds; ++i)
        {
            chA = digitalRead(CHA);
            chB = digitalRead(CHB);

            if(chA_past == chB_past && chA_past != chA)
              dblAngle += ANGLE_STEP;
            else if(chA_past == chB_past && chB_past != chB)
              dblAngle -= ANGLE_STEP;

            if(chA_past != chB_past && chB != chB_past)
              dblAngle += ANGLE_STEP;
            else if(chA_past != chB_past && chA != chA_past)
              dblAngle -= ANGLE_STEP;

            chA_past = chA;
            chB_past = chB;
            QThread::usleep(80);
        }
        resultAgl.push_back(dblAngle);
    }

    emit resultReady(resultAgl);

}


/*
// Constructors
Encoder::Encoder(std::vector<double> *phi, std::vector<double>* v, std::vector<double> *a){
    angle = phi;
    speed = v;
    acceleration = a;
}


// Memberfunctions

void Encoder::operator()(int sleep_time_micros, bool& go){
  stepSize = 1.0/50.0;
  double buffer;
  std::cout << "Start Measuring" << std::endl;

  size_t roundCounter =
    (int)round((pow(10,6)/sleep_time_micros)/50);
  while(go){
    // Messwerte 30 mal pro Sekunde
    for(size_t i = 0; i < roundCounter;++i){
      buffer = measure();
      //std::this_thread::sleep_for(std::chrono::microseconds(sleep_time_micros));
    }
  }
}

void Encoder::addCalcValue(double &mVal){
  // angle
  angle->push_back(mVal);
  currentAngle = mVal;

  // speed
  if(angle->size() >= 3){
    double f1 = *(angle->end()-1);
    double f2 = *(angle->end()-3);
    speed->push_back((f1-f2)/(stepSize*2));
    currentSpeed = speed->back();
  }

  // acceleration
  if(speed->size() >= 3){
    auto f1 = *(speed->end()-1);
    auto f2 = *(speed->end()-3);
    acceleration->push_back((f1-f2)/(stepSize*2));
    currentAcc = acceleration->back();
  }
}

double Encoder::measure(){
  int chA = digitalRead(CHA);
  int chB = digitalRead(CHB);

  // Condition Conditions to determine direction and speed
  // Channel B is leading Channel A
  if(chA == chA_past && chB == chB_past)
    return mAngle;

  if(chA_past == chB_past && chA_past != chA)
    mAngle += ANGLE_STEP;
  else if(chA_past == chB_past && chB_past != chB)
    mAngle -= ANGLE_STEP;

  if(chA_past != chB_past && chB != chB_past)
    mAngle += ANGLE_STEP;
  else if(chA_past != chB_past && chA != chA_past)
    mAngle -= ANGLE_STEP;

  chA_past = chA;
  chB_past = chB;
  return mAngle;
}

void Encoder::clean(){
  currentAngle = 0.0;
  currentSpeed = 0.0;
  currentAcc = 0.0;
  angle->clear();
  speed->clear();
  acceleration->clear();
}


// Output Memberfunctions
double Encoder::getCurrentAngle(){ return currentAngle; }
double Encoder::getCurrentSpeed(){ return currentSpeed; }
double Encoder::getCurrentAcc(){ return currentAcc; }

std::vector<double>* Encoder::getAngleVector(){ return angle; }
std::vector<double>* Encoder::getSpeedVector(){ return speed; }
std::vector<double>* Encoder::getAccVector(){ return acceleration; }
*/
