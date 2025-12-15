#ifndef CLASSDETAIL_H
#define CLASSDETAIL_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class ClassDetail;
}

class ClassDetail : public QWidget
{
    Q_OBJECT

public:
    explicit ClassDetail(QWidget *parent = nullptr);
    ~ClassDetail();
    
    void setClassId(int classId);

signals:
    void showClassList();
    void showClassMembers(int classId, QString className);
    void showCreateExam(int classId);
    void showExamDetail(int examId);

private slots:
    void on_backButton_clicked();
    void on_membersButton_clicked();
    void on_createExamButton_clicked();
    void on_refreshButton_clicked();
    void handleExamItemClicked(int examId);
    void handleDeleteExam(int examId);

private:
    Ui::ClassDetail *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    QString currentClassName;
    
    void loadClassDetail();
    void loadExamsInClass();
    void populateExamList(const QJsonArray &exams);
};

#endif // CLASSDETAIL_H