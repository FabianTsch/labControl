#ifndef CHART0_H
#define CHART0_H

#endif // CHART0_H
#include <QVector>

#include <QtCharts/QChart>
#include <QtCore/QTimer>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class LineChart: public QChart
{
    Q_OBJECT
public:
    LineChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});
    virtual ~LineChart();

public slots:
    void handleTimeout(QVector<double>&, QVector<double>&,
                       QVector<double>&, QVector<double>&);
    void clear();

private:
    QLineSeries *m_series;
    QStringList m_titles;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    int pastPrint;
    qreal m_step;
    qreal m_x;
    qreal m_y;
};

// Bar Chart
class BarChart: public QChart
{
    Q_OBJECT
public:
    BarChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});
    virtual ~BarChart();

public slots:
    void handleTimeout(QVector<double>&,QVector<double>&,
                       QVector<double>&,QVector<double>&);
    void clear();

private:
    QBarSet *barCurrent;
    QBarSet *barVoltage;
    QBarSeries *seriesBar;
    QStringList categories;
    QBarCategoryAxis *axisX;
    QValueAxis *axisY;
    qreal m_step;
    qreal m_x;
    qreal m_y;
};
