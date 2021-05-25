#include "charts.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>


//ts Chart
LineChart::LineChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(0),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    pastPrint(0),
    m_step(0),
    m_x(0),
    m_y(0)

{
    m_series = new QLineSeries(this);
    QPen green(Qt::blue);
    green.setWidth(3);
    m_series->setPen(green);
    m_series->append(m_x, m_y);

    addSeries(m_series);

    addAxis(m_axisX,Qt::AlignBottom);
    addAxis(m_axisY,Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    m_axisX->setTickCount(5);
    m_axisX->setRange(0, 20);
    m_axisY->setRange(0, 4500);
}

LineChart::~LineChart()
{

}

void LineChart::handleTimeout(QVector<double>& angle, QVector<double>& speed,
                              QVector<double>& acceleration, QVector<double>& time)
{
    qreal dx = 0;
    qreal dy = 0;
    qreal tickX = plotArea().width() / m_axisX->tickCount();
    qreal tickY = plotArea().width() / m_axisY->tickCount();
    for(; pastPrint < speed.size(); pastPrint++)
    {
        m_series->append(time[pastPrint],speed[pastPrint]*60);
    }

    if(m_axisX->max() < time.back()){
        dx = tickX*2;
    }

    if(dx!=0 ||dy!=0){
        scroll(dx,dy);
    }
}

void LineChart::clear()
{
    m_series->clear();
    m_axisX->setRange(0,20);
    pastPrint = 0;
}


// Bar Chart

BarChart::BarChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    barCurrent(new QBarSet("Current")),
    barVoltage(new QBarSet("Voltage")),
    seriesBar(new QBarSeries),
    axisX(new QBarCategoryAxis),
    axisY(new QValueAxis)
{
    seriesBar->append(barVoltage);
    seriesBar->append(barCurrent);
    addSeries(seriesBar);
    setAnimationOptions(QChart::AllAnimations);

    categories << "Drive" << "Load";
    axisX->append(categories);
    addAxis(axisX,Qt::AlignBottom);
    seriesBar->attachAxis(axisX);

    axisY->setRange(0,7);
    addAxis(axisY,Qt::AlignLeft);
    seriesBar->attachAxis(axisY);
}

BarChart::~BarChart()
{

}

void BarChart::handleTimeout(QVector<double>& voltageDrive,QVector<double>& voltageLoad,
                             QVector<double>& currentDrive,QVector<double>& currentLoad)
{

    int num = 100;
    if(voltageDrive.size() > num){
        double vDrive = 0.0;
        double iDrive = 0.0;
        double vLoad = 0.0;
        double iLoad = 0.0;
        for(int i = 1; i <= num; i++){
            vDrive += *(voltageDrive.end()-i);
            vLoad += *(voltageLoad.end()-i);
            iDrive += *(currentDrive.end()-i);
            iLoad += *(currentLoad.end()-i);
        }
        vDrive = vDrive/num;
        vLoad = vLoad/num;
        iDrive = iDrive/num;
        iLoad = iLoad/num;

        if(barVoltage->count()){
            barVoltage->replace(0,vDrive);
            barVoltage->replace(1,vLoad);
            barCurrent->replace(0,iDrive);
            barCurrent->replace(1,iLoad);

        }else{
            *barVoltage << vDrive << vLoad;
            *barCurrent << iDrive << iDrive;
        }
    }

}

void BarChart::clear()
{
    barVoltage->replace(0,0);
    barVoltage->replace(1,0);
    barCurrent->replace(0,0);
    barCurrent->replace(1,0);
}
