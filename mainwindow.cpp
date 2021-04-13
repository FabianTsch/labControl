#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Buttons
    QPushButton *start = MainWindow::findChild<QPushButton *>("Start");
    QPushButton *stop = MainWindow::findChild<QPushButton *>("Stop");
    QPushButton *apply = MainWindow::findChild<QPushButton *>("Apply");

    // Bts7960
    int drivePins[] = {2,1,4,5};
    int loadPins[] = {22,26,27,23};
    drive = new Bts7960_sPWM(drivePins,this);
    load = new Bts7960_sPWM(loadPins, this);
    connect(start,&QAbstractButton::released, load, &Bts7960_sPWM::startPressed);
    connect(start,&QAbstractButton::released, drive, &Bts7960_sPWM::startPressed);
    connect(stop,&QAbstractButton::released, load, &Bts7960_sPWM::stopPressed);
    connect(stop,&QAbstractButton::released, drive, &Bts7960_sPWM::stopPressed);
    connect(apply,&QAbstractButton::released, load, &Bts7960_sPWM::applyPressed);
    connect(apply,&QAbstractButton::released, drive, &Bts7960_sPWM::applyPressed);


    // Threads and Timer Encoder
    timer = new QTimer(this);
    Encoder *enc = new Encoder();
    enc->moveToThread(&workThread);
    connect(&workThread, &QThread::finished, enc, &QObject::deleteLater);
    connect(start, &QAbstractButton::released, this, &MainWindow::startPressed);
    connect(enc, &Encoder::resultReady, this, &MainWindow::handleEncoderResults);
    connect(this, &MainWindow::runMeasure, enc, &Encoder::measure);
    workThread.start();

    // LineChart
    QLineSeries *seriesLine = new QLineSeries();
    seriesLine->append(0, 16);
    seriesLine->append(1, 25);
    seriesLine->append(2, 16);
    seriesLine->append(3, 19);
    seriesLine->append(4, 70);
    seriesLine->append(5, 25);
    seriesLine->append(6, 34);

    QChart *chartLine = new QChart();
    chartLine->legend()->setVisible(true);
    chartLine->addSeries(seriesLine);
    chartLine->createDefaultAxes();

    QFont font;
    font.setPixelSize(18);
    chartLine->setTitleFont(font);
    chartLine->setTitle("But How long");

    QPen pen(QRgb(0x000000));
    pen.setWidth(5);
    seriesLine->setPen(pen);

    chartLine->setAnimationOptions(QChart::AllAnimations);

    // BarChart
    QBarSet *set0 = new QBarSet("jane");
    QBarSet *set1 = new QBarSet("Axel");
    QBarSet *set2 = new QBarSet("Mary");
    QBarSet *set3 = new QBarSet("John");
    QBarSet *set4 = new QBarSet("Fuckboy");


    *set0 << 10 << 20 << 30 << 40 << 50 << 60;
    *set1 << 50 << 70 << 40 << 45 << 80 << 70;
    *set2 << 30 << 50 << 80 << 13 << 80 << 50;
    *set3 << 50 << 60 << 70 << 30 << 40 << 25;
    *set4 << 90 << 70 << 50 << 30 << 16 << 42;

    QBarSeries *seriesBar = new QBarSeries();

    seriesBar->append(set0);
    seriesBar->append(set1);
    seriesBar->append(set2);
    seriesBar->append(set3);
    seriesBar->append(set4);

    QChart *chartBar = new QChart();
    chartBar->addSeries(seriesBar);
    chartBar->setTitle("Studen Performance");
    chartBar->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chartBar->createDefaultAxes();
    //chartBar->setAxisX(axis,seriesBar);


    // Render Charts
    chartViewLine = new QChartView(chartLine);
    chartViewLine->setRenderHint(QPainter::Antialiasing);
    chartViewLine->setParent(ui->chart0);

    chartViewBar = new QChartView(chartBar);
    chartViewBar->setRenderHint(QPainter::Antialiasing);
    chartViewBar->setParent(ui->chart1);


}

MainWindow::~MainWindow()
{
    delete ui;
    workThread.quit();
    workThread.wait();
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    chartViewBar->resize(chartViewBar->parentWidget()->size());
    chartViewLine->resize(chartViewLine->parentWidget()->size());
}

void MainWindow::startPressed()
{
    emit runMeasure();
}

void MainWindow::handleEncoderResults(const QVector<double>& resultAgl)
{
    double d = angle.back();
    int size = resultAgl.size();
    for(auto it = resultAgl.begin(); it < resultAgl.end(); ++it){
        angle.push_back(*it+d);
    }

    // Calculate
    // speed
      for(int i = 1; i <= size; i++){
          double f1 = *(angle.end()-i);
          double f2;
          if(angle.end()-(i+2)<angle.begin())
            f2 = 0;
          else f2 = *(angle.end()-(i+2));
          speed.push_back((f1-f2)/(stepSize*2));
      }


    // acceleration

      for(int i = 1; i <= size; i++){
          double f1 = *(speed.end()-i);
          double f2;
          if(speed.end()-(i+2)<speed.begin())
            f2 = 0;
          else f2 = *(speed.end()-(i+2));
          acceleration.push_back((f1-f2)/(stepSize*2));
      }


    //Print
    ui->outAngle->setText(QString::number(angle.back()/360));
    ui->outSpeed->setText(QString::number(speed.back()*60/360));
    ui->outAcceleration->setText(QString::number(acceleration.back()*60*60/360));

    // Ask for Start again
    if(load->checkRunning() && drive->checkRunning()){
        emit runMeasure();
    }
}


void MainWindow::on_Start_released()
{
    // Grab Input Information from ui to load and drive
    if(ui->selectInertiaLoad->isChecked()){
        load->enabelInertiaContoller(true);
    }else{
        load->enabelInertiaContoller(false);
    }
    if(ui->directionLeftDrive->isChecked()) drive->setDirection('l');
    if(ui->directionRightDrive->isChecked()) drive->setDirection('r');
    if(ui->directionLeftLoad->isChecked()) load->setDirection('l');
    if(ui->directionRightLoad->isChecked()) load->setDirection('r');
    drive->setDutyCycle(ui->dutyCycleDrive->value());
    load->setDutyCycle(ui->dutyCycleLoad->value());
    load->setInertiaLoad(ui->inertiaLoad->value());

}


void MainWindow::on_Apply_released()
{
    // Grab Input Information from ui to load and drive
    if(ui->selectInertiaLoad->isChecked()){
        load->enabelInertiaContoller(true);
    }else{
        load->enabelInertiaContoller(false);
    }
    if(ui->directionLeftDrive->isChecked()) drive->setDirection('l');
    if(ui->directionRightDrive->isChecked()) drive->setDirection('r');
    if(ui->directionLeftLoad->isChecked()) load->setDirection('l');
    if(ui->directionRightLoad->isChecked()) load->setDirection('r');
    drive->setDutyCycle(ui->dutyCycleDrive->value());
    load->setDutyCycle(ui->dutyCycleLoad->value());
    load->setInertiaLoad(ui->inertiaLoad->value());
}
