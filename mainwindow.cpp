#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QRandomGenerator>



using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , g(9.81)
    , r(27.2)
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

    int drivePins[] = {27,18,23,24};
    int loadPins[] = {6,14,16,15};
    drive = new Bts7960_sPWM(pi,drivePins,this);
    load = new Bts7960_sPWM(pi,loadPins, this);

    pinMode(2,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(22,OUTPUT);
    pinMode(27,OUTPUT);
    digitalWrite(2,1);
    digitalWrite(4,1);
    digitalWrite(22,1);
    digitalWrite(27,1);


    connect(start,&QAbstractButton::released, load, &Bts7960_sPWM::startPressed);
    connect(start,&QAbstractButton::released, drive, &Bts7960_sPWM::startPressed);
    connect(stop,&QAbstractButton::released, load, &Bts7960_sPWM::stopPressed);
    connect(stop,&QAbstractButton::released, drive, &Bts7960_sPWM::stopPressed);
    connect(apply,&QAbstractButton::released, load, &Bts7960_sPWM::applyPressed);
    connect(apply,&QAbstractButton::released, drive, &Bts7960_sPWM::applyPressed);


    // Encoder
    //Encoder *enc = new Encoder(pi,22,25);
    EncoderControl *enc = new EncoderControl(this);

    encoderTimer = new QTimer(this);
    intervalEncoder = 900;
    encoderTimer->setInterval(intervalEncoder);

    connect(encoderTimer, &QTimer::timeout, enc, &EncoderControl::getResults);
    connect(enc,&EncoderControl::resultReady, this, &MainWindow::handleEncoderResults);
    connect(start, &QAbstractButton::released, this, &MainWindow::startPressed);
    connect(this,&MainWindow::clear, enc, &EncoderControl::clear);




    //daqhat
    timer = new QTimer(this); // Timer start in MainWindow::startPressed
    intervalDAQ = 200;
    timer->setInterval(intervalDAQ);
    Daqhat *daqhat = new Daqhat(this);
    connect(start, &QAbstractButton::released, daqhat, &Daqhat::startPressed);
    connect(timer, &QTimer::timeout, daqhat, &Daqhat::getResults);
    connect(daqhat, &Daqhat::resultsReady, this, &MainWindow::handleDaqhatsResults);
    connect(stop, &QAbstractButton::released, daqhat, &Daqhat::stopPressed);

    // CHARTS
    chartTimer = new QTimer(this);
    chartTimer->setInterval(3000);


    // LineChart
    tsChart = new LineChart();
    tsChart->setTitle("Speed-Chart");
    tsChart->legend()->hide();
    //tsChart->setAnimationOptions(QChart::AllAnimations); // enable for Animation

    // BarChart
    barChart = new BarChart();
    barChart->setTitle("Voltage Current Bar Chart");
    barChart->setAnimationOptions(QChart::AllAnimations);


    connect(chartTimer, &QTimer::timeout, this, &MainWindow::synchroniseCharts);
    connect(this, &MainWindow::timeoutBarChart, barChart, &BarChart::handleTimeout);
    connect(this, &MainWindow::timeoutLineChart, tsChart, &LineChart::handleTimeout);
    connect(this, &MainWindow::clear, barChart, &BarChart::clear);
    connect(this, &MainWindow::clear, tsChart, &LineChart::clear);


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

int MainWindow::loadDataInString()
{
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
                QString::number(voltageDrive[i]*2) + ";" +
                QString::number(voltageLoad[i]*2) + ";" +
                QString::number(currentDrive[i]*10-25) + ";" +
                QString::number(currentLoad[i]*10-25) + "\n";
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

void MainWindow::handleEncoderResults(int steps, float loadCell1, float loadCell2)
{
    // Torque
    loadCell_1.push_back(loadCell1*g*r);
    loadCell_2.push_back(loadCell2+g+r);

    // Angle
    double phi = (double)steps/256.0;
    double interval_s = (double)intervalEncoder/1000.0;
    angle.push_back(phi);

    // Time
    if(encoderTimeS.size()< 1)
        encoderTimeS.push_back(0);
    encoderTimeS.push_back(encoderTimeS.back()+interval_s);

    // Speed
    if(angle.size() == 1) return;
    if(angle.size() == 2){
        speed.push_back((angle[1]-angle[0])/interval_s);
    }
    else
    {
        auto pos0 = angle.end()-1;
        auto pos1 = angle.end()-3;
        speed.push_back((*pos0-*pos1)/interval_s);
    }

    // acceleration
    if(speed.size() == 1) return;
    if(speed.size() == 2){
        acceleration.push_back((speed[1]-speed[0])/interval_s);
    }
    else
    {
        auto pos0 = speed.end()-1;
        auto pos1 = speed.end()-3;
        acceleration.push_back((*pos0-*pos1)/interval_s);
    }
    //Print
    ui->outAngle->setText(QString::number(angle.back()));
    ui->outSpeed->setText(QString::number(speed.back()*60));
    //ui->outAcceleration->setText(QString::number(acceleration.back()));

    if(loadCell1 > loadCell2)
        ui->outTorque->setText(QString::number(loadCell1*g*r/1000));
    else
        ui->outTorque->setText(QString::number(loadCell2*g*r/1000));

}

void MainWindow::startPressed()
{
    timer->start();
    chartTimer->start();
    encoderTimer->start();
}


void MainWindow::handleDaqhatsResults(const QVector<double>& results, int num_channels,
                                      int samples_read_per_channel)
{
    // Emit Time
    double samples = samples_read_per_channel;
    double interval = (double)intervalDAQ/1000.0;
    double index = interval/samples;
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
    double avgOut[4] = {0.0};
    int num = 300;
    for(int i = 1; i <= num; i++){
        avgOut[0] += *(currentDrive.end()-i);
        avgOut[1] += *(currentLoad.end()-i);
        avgOut[2] += *(voltageDrive.end()-i);
        avgOut[3] += *(voltageLoad.end()-i);
    }
    for(int i = 0; i < 4; i++){
        avgOut[i] = avgOut[i]/num;
    }
    if(samples_read_per_channel>0){
        ui->outCurrentDrive->setText(QString::number(avgOut[0]*10-25));
        ui->outCurrentLoad->setText(QString::number(avgOut[1]*10-25));
        ui->outVoltageDrive->setText(QString::number(avgOut[2]*2));
        ui->outVoltageLoad->setText(QString::number(avgOut[3]*2));
    }
}

void MainWindow::synchroniseCharts(){
    emit timeoutBarChart(voltageDrive,voltageLoad,currentDrive,currentLoad);
    emit timeoutLineChart(angle, speed, acceleration, encoderTimeS);
}

// Buttons
void MainWindow::on_Start_released()
{
    // Grab Input Information from ui to load and drive
    if(ui->directionLeftDrive->isChecked()) drive->setDirection('l');
    if(ui->directionRightDrive->isChecked()) drive->setDirection('r');
    if(ui->directionLeftLoad->isChecked()) load->setDirection('l');
    if(ui->directionRightLoad->isChecked()) load->setDirection('r');
    drive->setDutyCycle(ui->dutyCycleDrive->value());
    load->setDutyCycle(ui->dutyCycleLoad->value());

}

void MainWindow::on_Apply_released()
{
    // Grab Input Information from ui to load and drive
    if(ui->directionLeftDrive->isChecked()) drive->setDirection('l');
    if(ui->directionRightDrive->isChecked()) drive->setDirection('r');
    if(ui->directionLeftLoad->isChecked()) load->setDirection('l');
    if(ui->directionRightLoad->isChecked()) load->setDirection('r');
    drive->setDutyCycle(ui->dutyCycleDrive->value());
    load->setDutyCycle(ui->dutyCycleLoad->value());
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
    encoderTimer->stop();
}

void MainWindow::on_captureData_clicked()
{
    if(capturedData.isEmpty())
        capturedData = "V_drive / V;V_load / V;I_drive;I_load;revolutions;Speed / rpm;Torque / Nmm\n";
    capturedData +=
            QString::number(voltageDrive.back()*2) + ";" +
            QString::number(voltageLoad.back()*2) + ";" +
            QString::number(currentDrive.back()*2*10-25) + ";" +
            QString::number(currentLoad.back()*10-25) + ";" +
            QString::number(angle.back()) + ";" +
            QString::number(speed.back()*60) + ";";
    if(loadCell_1.back() > loadCell_2.back())
        capturedData += QString::number(loadCell_1.back()) + "\n";
    else
        capturedData += QString::number(loadCell_2.back()) + "\n";
}

void MainWindow::on_actionSave_triggered()
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
    out << capturedData;
    file.close();

}

void MainWindow::on_actionclear_all_triggered()
{
    // clear all Vectors
    data.clear();
    capturedData.clear();
    angle.clear();
    speed.clear();
    acceleration.clear();
    encoderTimeS.clear();
    voltageLoad.clear();
    voltageDrive.clear();
    currentLoad.clear();
    currentDrive.clear();
    daqTimeS.clear();
    loadCell_1.clear();
    loadCell_2.clear();

    // clean ui
    ui->outAngle->setText("");
    ui->outSpeed->setText("");
    //ui->outAcceleration->setText("");
    ui->outTorque->setText("");
    ui->outVoltageDrive->setText("");
    ui->outVoltageLoad->setText("");
    ui->outCurrentDrive->setText("");
    ui->outCurrentLoad->setText("");


    emit clear();
}
