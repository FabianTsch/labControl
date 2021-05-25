#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <wiringPi.h>
#include "daqhats_utils.h"
#include <pigpiod_if2.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wiringPiSetupGpio();
    MainWindow w;
    w.resize(960,1000);
    w.show();
    return a.exec();
}
