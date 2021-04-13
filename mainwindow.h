#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>

// Threads, Timer and Hardware Objects
#include<QThread>
#include<QTimer>
#include<encoder.h>
#include<BTS7960.h>

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
    QThread workThread;
    Bts7960_sPWM *load;
    Bts7960_sPWM *drive;

protected:
    void resizeEvent(QResizeEvent* event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QChartView *chartViewLine;
    QChartView *chartViewBar;
    QVector<double> angle;
    QVector<double> speed;
    QVector<double> acceleration;
    const double stepSize = 1.0/50.0;


public slots:
    void startPressed();
    void handleEncoderResults(const QVector<double>&);

signals:
    void runMeasure();


private slots:
    void on_Start_released();
    void on_Apply_released();
};
#endif // MAINWINDOW_H
