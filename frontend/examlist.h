#ifndef EXAMLIST_H
#define EXAMLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonArray>

namespace Ui {
class ExamList;
}

class ExamList : public QWidget
{
    Q_OBJECT

public:
    explicit ExamList(QWidget *parent = nullptr);
    ~ExamList();
    
    void loadExams();

signals:
    void backToClassList();
    void startExam(int examId, QString examName, int timeLimit);
    void viewExamResult(int submissionId);

private slots:
    void onReadyRead();
    void onExamClicked(int examId);

private:
    Ui::ExamList *ui;
    QTcpSocket *tcpSocket;
    QJsonArray exams;
    
    void displayExams();
};

#endif // EXAMLIST_H

