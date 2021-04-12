#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Buttons
    QPushButton *start = MainWindow::findChild<QPushButton *>("Start");

    // Bts7960
    //drive = new Bts7960_sPWM(this);
    //load = new Bts7960(this);




    // Threads and Timer Encoder
    timer = new QTimer(this);
    Encoder *enc = new Encoder();
    enc->moveToThread(&workThread);
    connect(&workThread, &QThread::finished, enc, &QObject::deleteLater);
    connect(start, &QAbstractButton::released, this, &MainWindow::startPressed);
    connect(timer, &QTimer::timeout, enc, &Encoder::measure);
    connect(enc, &Encoder::resultReady, this, &MainWindow::handleEncoderResults);
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
    timer->start(220);
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
          double f1 = *(angle.end()-1);
          double f2;
          if(angle.end()-(i+2)<angle.begin())
            f2 = 0;
          else f2 = *(angle.end()-(i+2));
          speed.push_back((f1-f2)/(stepSize*2));
      }


    // acceleration

      for(int i = 1; i <= size; i++){
          double f1 = *(speed.end()-1);
          double f2;
          if(speed.end()-(i+2)<speed.begin())
            f2 = 0;
          else f2 = *(speed.end()-(i+2));
          acceleration.push_back((f1-f2)/(stepSize*2));
      }


    //Print
    ui->outAngle->setText(QString::number(angle.back()/360));
    ui->outSpeed->setText(QString::number(speed.back()/360));
    ui->outAcceleration->setText(QString::number(acceleration.back()/360));
}


void MainWindow::on_Start_released()
{
    /*
    if(ui->directionRight->isChecked()){
        drive->setDirection('r');
        load->setDirection('l');
    }else if(ui->directionLeft->isChecked()){
        drive->setDirection('l');
        load->setDirection('r');
    }
    if(ui->selectInertiaLoad->isChecked()){
        load->enabelInertiaContoller(true);
    }else{
        load->enabelInertiaContoller(false);
    }
    ui->inertiaLoad->setText("HelloWorld");
    */
}

