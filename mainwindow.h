#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QCameraInfo>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QUrl>


// Threads, Timer and Hardware Objects
#include<QThread>
#include<QTimer>
#include<encoder.h>
#include<BTS7960.h>
#include<daqhat.h>
#include"charts.h"

// Charts
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace QtCharts;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTimer* timer;
    QTimer* chartTimer;
    QThread workThread;
    Bts7960_sPWM *load;
    Bts7960_sPWM *drive;
    QChart *chartLine;
    QLineSeries *seriesLine;
    QValueAxis *LineAxisX;
    QValueAxis *LineAxisY;
    LineChart *tsChart;
    BarChart *barChart;


protected:
    void resizeEvent(QResizeEvent* event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectCamera();


private:
    Ui::MainWindow *ui;
    QCamera* camera;
    QCameraViewfinder* viewfinder;
    QCameraImageCapture* imageCapture;
    QMediaRecorder* recorder;
    bool connected;
    bool recording;
    QChartView *chartViewLine;
    QChartView *chartViewBar;
    QVector<double> angle;
    QVector<double> speed;
    QVector<double> acceleration;
    QVector<double> voltageLoad;
    QVector<double> voltageDrive;
    QVector<double> currentLoad;
    QVector<double> currentDrive;
    const double stepSize = 1.0/50.0;


public slots:
    void startPressed();
    void handleEncoderResults(const QVector<double>&);
    void handleDaqhatsResults(const QVector<double>&, int, int);
    void synchroniseCharts();

signals:
    void runMeasure();
    void timeoutBarChart(QVector<double>& voltageDrive,QVector<double>& voltageLoad,
                         QVector<double>& currentDrive,QVector<double>& currentLoad);
    void timeoutLineChart();


private slots:
    void on_Start_released();
    void on_Apply_released();
    void on_connectButton_clicked();
    void on_captureButton_clicked();
    void on_recordButton_clicked();
};
#endif // MAINWINDOW_H
