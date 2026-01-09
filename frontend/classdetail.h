#ifndef CLASSDETAIL_H
#define CLASSDETAIL_H

#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QListWidgetItem>

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
    void backToClassList();
    void openCreateExam(int classId);
    void openClassMembers(int classId, QString className);
    void openExamDetail(int examId);

private slots:
    void on_backButton_clicked();
    void on_createExamButton_clicked();
    void on_membersButton_clicked();
    void loadClassDetail();
    void loadExamsInClass();
    void onExamItemClicked(QListWidgetItem *item);
    void on_refreshButton_clicked();

private:
    Ui::ClassDetail *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    QString currentClassName;
    
    QPushButton *questionBankBtn;
};

#endif // CLASSDETAIL_H
