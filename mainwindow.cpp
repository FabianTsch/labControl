#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QRandomGenerator>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MCI Laboratory");

    // Camera
    connected = false;
    camera = new QCamera();
    qDebug() << "Number of cameras found:" <<
                QCameraInfo::availableCameras().count();
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach(const QCameraInfo &cameraInfo, cameras)
    {
        qDebug() << "Camera info:" << cameraInfo.deviceName() <<
                    cameraInfo.description() << cameraInfo.position();
        ui->deviceSelection->addItem(cameraInfo.description());
    }

    // Buttons
    QPushButton *start = MainWindow::findChild<QPushButton *>("Start");
    QPushButton *stop = MainWindow::findChild<QPushButton *>("Stop");
    QPushButton *apply = MainWindow::findChild<QPushButton *>("Apply");

    // Bts7960
    //int drivePins[] = {2,1,4,5};  WiringPi
    //int loadPins[] = {22,26,27,23}; WiringPi
    pi = pigpio_start(NULL,NULL);
    qDebug() << gpio_write(pi,27,1) << "\n";
    qDebug() << gpio_write(pi,16,1) << "" << gpio_read(pi,16);

    int drivePins[] = {27,18,23,24};
    int loadPins[] = {6,14,16,15};
    drive = new Bts7960_sPWM(pi,drivePins,this);
    load = new Bts7960_sPWM(pi,loadPins, this);


    connect(start,&QAbstractButton::released, load, &Bts7960_sPWM::startPressed);
    connect(start,&QAbstractButton::released, drive, &Bts7960_sPWM::startPressed);
    connect(stop,&QAbstractButton::released, load, &Bts7960_sPWM::stopPressed);
    connect(stop,&QAbstractButton::released, drive, &Bts7960_sPWM::stopPressed);
    connect(apply,&QAbstractButton::released, load, &Bts7960_sPWM::applyPressed);
    connect(apply,&QAbstractButton::released, drive, &Bts7960_sPWM::applyPressed);


    // Threads and Encoder
    Encoder *enc = new Encoder();
    enc->moveToThread(&workThread);
    connect(&workThread, &QThread::finished, enc, &QObject::deleteLater);
    connect(start, &QAbstractButton::released, this, &MainWindow::startPressed);
    connect(enc, &Encoder::resultReady, this, &MainWindow::handleEncoderResults);
    connect(this, &MainWindow::runMeasure, enc, &Encoder::measure);
    workThread.start();


    //daqhat
    timer = new QTimer(this); // Timer start in MainWindow::startPressed
    intervalDAQ = 500;
    timer->setInterval(intervalDAQ);
    Daqhat *daqhat = new Daqhat(this);
    connect(start, &QAbstractButton::released, daqhat, &Daqhat::startPressed);
    connect(timer, &QTimer::timeout, daqhat, &Daqhat::getResults);
    connect(daqhat, &Daqhat::resultsReady, this, &MainWindow::handleDaqhatsResults);
    connect(stop, &QAbstractButton::released, daqhat, &Daqhat::stopPressed);

    // CHARTS
    chartTimer = new QTimer(this);
    chartTimer->setInterval(300);


    // Ts Chart
    tsChart = new LineChart();
    tsChart->setTitle("Torque-Speed diagramm");
    tsChart->setAnimationOptions(QChart::AllAnimations);

    // BarChart
    barChart = new BarChart();
    barChart->setTitle("Voltage Current Bar Chart");
    barChart->setAnimationOptions(QChart::AllAnimations);



    connect(chartTimer, &QTimer::timeout, this, &MainWindow::synchroniseCharts);
    connect(this, &MainWindow::timeoutBarChart, barChart, &BarChart::handleTimeout);
    connect(this, &MainWindow::timeoutLineChart, tsChart, &LineChart::handleTimeout);
    // Render Charts
    chartViewLine = new QChartView(tsChart);
    chartViewLine->setRenderHint(QPainter::Antialiasing);
    chartViewLine->setParent(ui->chart0);

    chartViewBar = new QChartView(barChart);
    chartViewBar->setRenderHint(QPainter::Antialiasing);
    chartViewBar->setParent(ui->chart1);

}

MainWindow::~MainWindow()
{
    delete ui;
    pigpio_stop(pi);
    workThread.quit();
    workThread.wait();
}

// Memberfunctions

void MainWindow::connectCamera(){
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach(const QCameraInfo &cameraInfo, cameras){
        qDebug() << cameraInfo.description() << ui->deviceSelection->currentText();
        if(cameraInfo.description() == ui->deviceSelection->currentText()){
            camera = new QCamera(cameraInfo);
            viewfinder = new QCameraViewfinder(this);
            camera->setViewfinder(viewfinder);
            ui->webcamlayout->addWidget(viewfinder);

            connected = true;
            ui->connectButton->setText("Disconnect");
            camera->start();

            return;

        }
    }
}

int MainWindow::loadDataInString(){
    // Error handeling
    if(load->checkRunning() || drive->checkRunning()){
        QMessageBox::warning(this, "Warning", "Drive still running");
        return -1;
    }
    bool checkSize = daqTimeS.size() == voltageDrive.size() &&
            daqTimeS.size() == voltageLoad.size() &&
            daqTimeS.size() == currentDrive.size() &&
            daqTimeS.size() == currentLoad.size();
    if(!checkSize){
        QMessageBox::warning(this, "Warning", "Vector are not same size");
        qDebug() << "Size\ntime:" << daqTimeS.size()
                 << "V Drive: " << voltageDrive.size()
                 << "V Load: " << voltageLoad.size()
                 << "I Drive: " << currentDrive.size()
                 << "I Load: " << currentLoad.size();
        return -1;
    }

    // load data in String
    data.clear();
    data = "Time / s; V_drive / V;V_load / V;I_drive;I_load;\n";
    for(int i = 0; i < daqTimeS.size(); i++){
        QString buf = QString::number(daqTimeS[i]) + ";" +
                QString::number(voltageDrive[i]) + ";" +
                QString::number(voltageLoad[i]) + ";" +
                QString::number(currentDrive[i] ) + ";" +
                QString::number(currentLoad[i] ) + "\n";
        data += buf;
    }
    return 0;
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    chartViewBar->resize(chartViewBar->parentWidget()->size());
    chartViewLine->resize(chartViewLine->parentWidget()->size());
}

// Slots

void MainWindow::handleEncoderResults(const QVector<double>& resultAgl)
{
    double d;
    if(angle.size() > 0)
        d = angle.back();
    else
        d = 0;

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

void MainWindow::startPressed()
{
    timer->start();
    chartTimer->start();
    emit runMeasure();
}


void MainWindow::handleDaqhatsResults(const QVector<double>& results, int num_channels,
                                      int samples_read_per_channel)
{
    // Emit Time
    double samples = samples_read_per_channel;
    double interval = (double)intervalDAQ/1000.0;
    double index = interval/samples;
    qDebug() << "Index: " << index << "samples: " << samples << "interval:" << interval;
    for(int i = 0; i < samples_read_per_channel; i++){
        if(daqTimeS.empty())
            daqTimeS.push_back(index);
        else
            daqTimeS.push_back(daqTimeS.back()+index);
    }

    // Emit Voltages
    for(int i = 0; i < (num_channels*samples_read_per_channel);){
        currentDrive.push_back(results.at(i++));
        currentLoad.push_back(results.at(i++));
        voltageDrive.push_back(results.at(i++));
        voltageLoad.push_back(results.at(i++));
    }

    // Print Life Values
    if(samples_read_per_channel>0){
        ui->outCurrentDrive->setText(QString::number(currentDrive.back()));
        ui->outCurrentLoad->setText(QString::number(currentLoad.back()));
        ui->outVoltageDrive->setText(QString::number(voltageDrive.back()));
        ui->outVoltageLoad->setText(QString::number(voltageLoad.back()));
    }
}

void MainWindow::synchroniseCharts(){
    emit timeoutBarChart(voltageDrive,voltageLoad,currentDrive,currentLoad);
    emit timeoutLineChart();
}

// Buttons
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

void MainWindow::on_connectButton_clicked()
{
    if(!connected){
       connectCamera();
    }else{
        camera->stop();
        viewfinder->deleteLater();
        ui->connectButton->setText("Connect");
        connected = false;
    }
}

void MainWindow::on_captureButton_clicked()
{
    if(connected){
        imageCapture = new QCameraImageCapture(camera);
        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->searchAndLock();
        imageCapture->capture(qApp->applicationDirPath());
        camera->unlock();
    }
}

void MainWindow::on_recordButton_clicked()
{
    if(connected){
        recorder = new QMediaRecorder(camera);
        camera->setCaptureMode(QCamera::CaptureVideo);
        recorder->setOutputLocation(QUrl(qApp->applicationDirPath()));
        recorder->record();
        recording = true;
    }else{
        recorder->stop();
        recording = false;
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
        return;
    }
    currentFile = fileName;
    QTextStream out(&file);
    if(loadDataInString())
        QMessageBox::warning(this,"Warning", "Could not load data in string");
    out << data;
    file.close();

}

void MainWindow::on_Stop_released()
{
    chartTimer->stop();
    timer->stop();
}
