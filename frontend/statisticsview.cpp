#include "statisticsview.h"
#include "ui_statisticsview.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
// #include <QtCharts/QChart>
// #include <QtCharts/QBarSeries>
// #include <QtCharts/QBarSet>
// #include <QtCharts/QBarCategoryAxis>
// #include <QtCharts/QValueAxis>
#include <QVBoxLayout>

StatisticsView::StatisticsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsView),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    
    connect(ui->btnBack, &QPushButton::clicked, this, &StatisticsView::backPressed);
}

StatisticsView::~StatisticsView()
{
    delete ui;
}

void StatisticsView::loadExamStatistics(int examId)
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["exam_id"] = examId;
        
        QString request = QString("CONTROL GET_EXAM_STATISTICS\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &StatisticsView::onReadyRead);
    }
}

void StatisticsView::loadClassStatistics(int classId)
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["class_id"] = classId;
        
        QString request = QString("CONTROL GET_CLASS_STATISTICS\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &StatisticsView::onReadyRead);
    }
}

void StatisticsView::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "Statistics response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    int jsonStart = responseStr.indexOf('{');
    if (jsonStart != -1) {
        QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
        QJsonObject data = doc.object();
        
        if (responseStr.contains("EXAM_STATISTICS")) {
            displayExamStats(data);
        } else if (responseStr.contains("CLASS_STATISTICS")) {
            displayClassStats(data);
        }
    }
}

void StatisticsView::displayExamStats(const QJsonObject &data)
{
    ui->lblTitle->setText("📊 Thống kê bài thi");
    
    // Summary info
    int totalStudents = data["total_students"].toInt();
    double avgScore = data["average_score"].toDouble();
    double minScore = data["min_score"].toDouble();
    double maxScore = data["max_score"].toDouble();
    
    QString summary = QString(
        "Số học sinh: %1 | Điểm TB: %2 | Điểm thấp nhất: %3 | Điểm cao nhất: %4")
        .arg(totalStudents)
        .arg(avgScore, 0, 'f', 1)
        .arg(minScore, 0, 'f', 1)
        .arg(maxScore, 0, 'f', 1);
    
    ui->lblSummary->setText(summary);
}

void StatisticsView::displayClassStats(const QJsonObject &data)
{
    ui->lblTitle->setText("📊 Thống kê lớp: " + data["class_name"].toString());
    
    int examCount = data["exam_count"].toInt();
    int studentCount = data["student_count"].toInt();
    int questionCount = data["question_count"].toInt();
    
    QString summary = QString(
        "Số bài thi: %1 | Số học sinh: %2 | Số câu hỏi: %3")
        .arg(examCount).arg(studentCount).arg(questionCount);
    
    ui->lblSummary->setText(summary);
    
    /* // Create bar chart for exam averages
    QJsonArray exams = data["exams"].toArray();
    
    QBarSet *set = new QBarSet("Điểm TB");
    QStringList categories;
    
    for (int i = 0; i < exams.size(); i++) {
        QJsonObject exam = exams[i].toObject();
        *set << exam["average_score"].toDouble();
        categories << exam["exam_name"].toString();
    }
    
    QBarSeries *series = new QBarSeries();
    series->append(set);
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Điểm trung bình các bài thi");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Điểm");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(false);
    
    // Clear old chart and add new
    QLayoutItem *child;
    while ((child = ui->chartLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    } */
}

