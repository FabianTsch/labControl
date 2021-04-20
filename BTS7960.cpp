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
    gpio_write(pi,R_en,1);
    gpio_write(pi,L_en,1);

    //check for Hardware support
    int hpin[] = {-1,-1,-1}; //18,13,12
    for(int i = 0; i < 3; i++){
        if(hpin[i]==R_pwm){
            for(int k = 0; k < 3; k++){
                if(hpin[k]==L_pwm)
                    hardwareSupportPwm = true;
            }
        }
    }
    // if there is hardware support it will be used
    // otherwise software pwm will be used
    if(hardwareSupportPwm){
        hardware_PWM(pi,R_pwm,hFrequency,0);
        hardware_PWM(pi,L_pwm,hFrequency,0);
    }else{
        //set_PWM_frequency(pi,L_pwm,sFrequency);
        //set_PWM_frequency(pi,R_pwm,sFrequency);
        set_PWM_dutycycle(pi,R_pwm,10000);
        set_PWM_dutycycle(pi,L_pwm,0);
    }
}


// Slots
void Bts7960_sPWM::startPressed(){
    running = true;
    gpio_write(pi,L_en,1);
    gpio_write(pi,R_en,1);
    int d;
    if(hardwareSupportPwm){
        d = (int)(dutyCycle*1000000);
        if(direction == 'r'){
            hardware_PWM(pi,L_pwm,hFrequency,0);
            hardware_PWM(pi,R_pwm,hFrequency,d);
        }
        if(direction == 'l'){
            hardware_PWM(pi,R_pwm,hFrequency,0);
            hardware_PWM(pi,L_pwm,hFrequency,d);
        }
    }else{
        d = (int)(dutyCycle*255);
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
void Bts7960_sPWM::applyPressed(){
    if(running)
    {
        int d;
        if(hardwareSupportPwm){
            d = (int)(dutyCycle*1000000);
            if(direction == 'r'){
                hardware_PWM(pi,L_pwm,hFrequency,0);
                hardware_PWM(pi,R_pwm,hFrequency,d);
            }
            if(direction == 'l'){
                hardware_PWM(pi,R_pwm,hFrequency,0);
                hardware_PWM(pi,L_pwm,hFrequency,d);
            }
        }else{
            d = (int)(dutyCycle*255);
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

}
void Bts7960_sPWM::stopPressed(){
    running = false;
    gpio_write(pi,R_en,0);
    gpio_write(pi,L_en,0);
    if(hardwareSupportPwm){
        hardware_PWM(pi,R_pwm,hFrequency,0);
        hardware_PWM(pi,L_pwm,hFrequency,0);
    }else{
        set_PWM_dutycycle(pi,R_pwm,0);
        set_PWM_dutycycle(pi,L_pwm,0);
    }
}

// Access functions:
void Bts7960_sPWM::setDirection(const char d){direction = d;}
void Bts7960_sPWM::enabelInertiaContoller(const bool &enable){ inertia =enable; }
void Bts7960_sPWM::setDutyCycle(const double &d){ dutyCycle = d; }
void Bts7960_sPWM::setInertiaLoad(const double & I){ dblInertia = I; }
bool Bts7960_sPWM::checkInertiaEnabled(){ return inertia; }
bool Bts7960_sPWM::checkRunning(){ return running; }


