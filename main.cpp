#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <wiringPi.h>
#include "daqhats_utils.h"
#include <pigpiod_if2.h>



int main(int argc, char *argv[])
{
    wiringPiSetup();
        int pi = pigpio_start(NULL,NULL);
        gpio_write(pi,27,1);
        gpio_write(pi,23,1);
        gpio_write(pi,6,1);
        gpio_write(pi,16,1);
        set_PWM_dutycycle(pi,18,125);
        set_PWM_frequency(pi,18,800);
        //std::cout << get_PWM_frequency(pi,18);
        //hardware_PWM(pi,18,100,50000);
        std::cout << std::endl << "Was soll das?" << std::endl;
        return 0;

//    QApplication a(argc, argv);
//    wiringPiSetup();
//    MainWindow w;
//    w.resize(960,1000);
//    w.show();
//    return a.exec();
}
