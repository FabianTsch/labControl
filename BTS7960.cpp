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

void Bts7960_sPWM::setDirection(const char d){direction = d;}

Bts7960_sPWM::Bts7960_sPWM(QObject* parent)
    : QObject(parent)
{
    R_en = 2;
    R_pwm = 1;
    L_en = 4;
    L_pwm = 5;
    pinMode(R_en,OUTPUT);
    pinMode(R_pwm,OUTPUT);
    pinMode(L_en,OUTPUT);
    pinMode(L_pwm,OUTPUT);
    digitalWrite(R_en, HIGH);
    digitalWrite(L_en, HIGH);
    softPwmCreate(R_pwm,0,100);
    softPwmCreate(L_pwm,0,100);
}

void Bts7960::setDirection(const char &d){direction = d;}
void Bts7960::enabelInertiaContoller(const bool &enable){ inertia = enable;}




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




void Bts7960_sPWM::setPower(int p){
  power = p;
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
