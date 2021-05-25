#include "BTS7960.h"

Bts7960_sPWM::Bts7960_sPWM(int addr,int *pins, QObject* parent)
    : QObject(parent)
{
    pi = addr;
    R_en = pins[0];
    R_pwm = pins[1];
    L_en = pins[2];
    L_pwm = pins[3];
    running = false;
    hardwareSupportPwm = false;
    
    //test = set_mode(pi,R_en,PI_OUTPUT);
    //test = set_mode(pi,L_en,PI_OUTPUT);
    pinMode(R_en,OUTPUT);
    pinMode(L_en,OUTPUT);
    digitalWrite(R_en,0);
    digitalWrite(L_en,0);

    set_PWM_frequency(pi,L_pwm,sFrequency);
    set_PWM_frequency(pi,R_pwm,sFrequency);
    set_PWM_dutycycle(pi,R_pwm,0);
    set_PWM_dutycycle(pi,L_pwm,0);

}


// Slots
void Bts7960_sPWM::startPressed(){
    running = true;
    int d;
    d = (int)(dutyCycle*255);
    // make sure Drive disable if not running
    if(d)
    {
        digitalWrite(R_en,1);
        digitalWrite(L_en,1);
    }
    else{
        digitalWrite(R_en,0);
        digitalWrite(L_en,0);
    }
    if(direction == 'r'){
        set_PWM_dutycycle(pi,L_pwm,0);
        set_PWM_dutycycle(pi,R_pwm,d);
    }
    if(direction == 'l'){
        set_PWM_dutycycle(pi,R_pwm,0);
        set_PWM_dutycycle(pi,L_pwm,d);
    }
}
void Bts7960_sPWM::applyPressed(){
    int d;
    if(running)
    {
        int d;
        d = (int)(dutyCycle*255);
        // make sure Drive disable if not running
        if(d)
        {
            digitalWrite(R_en,1);
            digitalWrite(L_en,1);
        }
        else{
            digitalWrite(R_en,0);
            digitalWrite(L_en,0);
        }
        if(direction == 'r'){
            set_PWM_dutycycle(pi,L_pwm,0);
            set_PWM_dutycycle(pi,R_pwm,d);
        }
        if(direction == 'l'){
            set_PWM_dutycycle(pi,R_pwm,0);
            set_PWM_dutycycle(pi,L_pwm,d);
        }
    }

}
void Bts7960_sPWM::stopPressed(){
    running = false;
    gpio_write(pi,R_en,0);
    gpio_write(pi,L_en,0);

    set_PWM_dutycycle(pi,R_pwm,0);
    set_PWM_dutycycle(pi,L_pwm,0);

}

// Access functions:
void Bts7960_sPWM::setDirection(const char d){direction = d;}
void Bts7960_sPWM::enabelInertiaContoller(const bool &enable){ inertia =enable; }
void Bts7960_sPWM::setDutyCycle(const double &d){ dutyCycle = d; }
void Bts7960_sPWM::setInertiaLoad(const double & I){ dblInertia = I; }
bool Bts7960_sPWM::checkInertiaEnabled(){ return inertia; }
bool Bts7960_sPWM::checkRunning(){ return running; }


