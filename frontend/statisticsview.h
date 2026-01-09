#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QTcpSocket>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

namespace Ui {
class StatisticsView;
}

class StatisticsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView();
    
    void loadExamStatistics(int examId);
    void loadClassStatistics(int classId);

signals:
    void backPressed();

private slots:
    void onReadyRead();

private:
    Ui::StatisticsView *ui;
    QTcpSocket *tcpSocket;
    
    void displayExamStats(const QJsonObject &data);
    void displayClassStats(const QJsonObject &data);
};

#endif // STATISTICSVIEW_H

