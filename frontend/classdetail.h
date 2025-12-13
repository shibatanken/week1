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

private slots:
    void on_backButton_clicked();
    void on_addStudentButton_clicked();
    void on_refreshButton_clicked();
    void handleRemoveStudent(int userId);

private:
    Ui::ClassDetail *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    
    void loadClassDetail();
    void loadStudentsInClass();
    void loadStudentsNotInClass();
    void populateStudentList(const QJsonArray &students, bool inClass);
};

#endif // CLASSDETAIL_H

