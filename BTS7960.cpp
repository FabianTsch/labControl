#include "BTS7960.h"

Bts7960::Bts7960(QObject* parent)
    : QObject(parent)
{
    R_en = 22;
    R_pwm = 26;
    L_en = 27;
    L_pwm = 23;
    pinMode(R_en,OUTPUT);
    pinMode(R_pwm,PWM_OUTPUT);
    pinMode(L_en,OUTPUT);
    pinMode(L_pwm,PWM_OUTPUT);
    digitalWrite(R_en, HIGH);
    digitalWrite(L_en, HIGH);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(100);
    pwmSetClock(1024);
    pwmWrite(L_pwm,0);
    pwmWrite(R_pwm,0);

}
void Bts7960::setDirection(const char &d){direction = d;}


Bts7960_sPWM::Bts7960_sPWM(int *pins, QObject* parent)
    : QObject(parent)
{
    R_en = pins[0];
    R_pwm = pins[1];
    L_en = pins[2];
    L_pwm = pins[3];
    running = false;

    pinMode(R_en,OUTPUT);
    pinMode(R_pwm,OUTPUT);
    pinMode(L_en,OUTPUT);
    pinMode(L_pwm,OUTPUT);
    digitalWrite(R_en, HIGH);
    digitalWrite(L_en, HIGH);
    softPwmCreate(R_pwm,0,100);
    softPwmCreate(L_pwm,0,100);
}


// Slots
void Bts7960_sPWM::startPressed(){
    running = true;
    digitalWrite(L_en,1);
    digitalWrite(R_en,1);
    int d = (int)(dutyCycle*100);
    if(direction == 'r'){
        softPwmWrite(L_pwm,0);
        softPwmWrite(R_pwm,d);
    }
    if(direction == 'l'){
        softPwmWrite(R_pwm,0);
        softPwmWrite(L_pwm,d);
    }
}
void Bts7960_sPWM::applyPressed(){
    if(running)
    {
        int d = (int)(dutyCycle*100);
        if(direction == 'r'){
            softPwmWrite(L_pwm,0);
            softPwmWrite(R_pwm,d);
        }
        if(direction == 'l'){
            softPwmWrite(R_pwm,0);
            softPwmWrite(L_pwm,d);
        }
    }

}
void Bts7960_sPWM::stopPressed(){
    running = false;
    digitalWrite(L_en,0);
    digitalWrite(R_en,0);
    softPwmWrite(L_pwm,0);
    softPwmWrite(R_pwm,0);
}

// Access functions:
void Bts7960_sPWM::setDirection(const char d){direction = d;}
void Bts7960_sPWM::enabelInertiaContoller(const bool &enable){ inertia =enable; }
void Bts7960_sPWM::setDutyCycle(const double &d){ dutyCycle = d; }
void Bts7960_sPWM::setInertiaLoad(const double & I){ dblInertia = I; }
bool Bts7960_sPWM::checkInertiaEnabled(){ return inertia; }
bool Bts7960_sPWM::checkRunning(){ return running; }





/*
void Bts7960::setPower(int p){
  power = p;
}

void Bts7960::start(char d){
  switch(d){
    case 'r': std::cout << "Start nach rechts" << std::endl; pwmWrite(L_pwm,0); delay(5); pwmWrite(R_pwm, power); break;
    case 'l': std::cout << "Start nach links" << std::endl; pwmWrite(R_pwm,0); delay(5); pwmWrite(L_pwm, power); break;
    default: pwmWrite(L_pwm,0); delay(5); pwmWrite(R_pwm, power); break;
  }
}

void Bts7960::stop(){
  pwmWrite(L_pwm,0);
  pwmWrite(R_pwm,0);
}

void Bts7960::checkPin(){
  std::cout << "R_is: " << R_is << std::endl;
  std::cout << "R_en: " << R_en << std::endl;
  std::cout << "R_pwm: " << R_pwm << std::endl;
  std::cout << "L_is: " << L_is << std::endl;
  std::cout << "L_en: " << L_en << std::endl;
  std::cout << "L_pwm: " << L_pwm << std::endl;
}






void Bts7960_sPWM::start(char d){
  switch(d){
    case 'r': std::cout << "Start nach rechts" << std::endl; softPwmWrite(L_pwm,0); delay(5); softPwmWrite(R_pwm, power); break;
    case 'l': std::cout << "Start nach links" << std::endl; softPwmWrite(R_pwm,0); delay(5); softPwmWrite(L_pwm, power); break;
    default: softPwmWrite(L_pwm,0); delay(5); softPwmWrite(R_pwm, power); break;
  }
}

void Bts7960_sPWM::stop(){
  softPwmWrite(L_pwm,0);
  softPwmWrite(R_pwm,0);
}

void Bts7960_sPWM::checkPin(){
  std::cout << "R_is: " << R_is << std::endl;
  std::cout << "R_en: " << R_en << std::endl;
  std::cout << "R_pwm: " << R_pwm << std::endl;
  std::cout << "L_is: " << L_is << std::endl;
  std::cout << "L_en: " << L_en << std::endl;
  std::cout << "L_pwm: " << L_pwm << std::endl;
}
*/
