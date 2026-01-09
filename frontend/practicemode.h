#ifndef PRACTICEMODE_H
#define PRACTICEMODE_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonArray>

namespace Ui {
class PracticeMode;
}

class PracticeMode : public QWidget
{
    Q_OBJECT

public:
    explicit PracticeMode(QWidget *parent = nullptr);
    ~PracticeMode();
    
    void setClassInfo(int classId, QString className);

signals:
    void backToClassDetail();

private slots:
    void onReadyRead();
    void onStartPractice();
    void onAnswerSelected(QString answer);
    void onNextQuestion();
    void onFinishPractice();

private:
    Ui::PracticeMode *ui;
    QTcpSocket *tcpSocket;
    
    int classId;
    QString className;
    int sessionId;
    int currentQuestionIndex;
    QJsonArray questions;
    
    void loadPracticeQuestions();
    void displayQuestion(int index);
    void showResult(const QJsonObject &result);
};

#endif // PRACTICEMODE_H

