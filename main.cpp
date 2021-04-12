#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <wiringPi.h>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wiringPiSetup();
    MainWindow w;
    w.resize(960,1000);
    w.show();
    return a.exec();
}
