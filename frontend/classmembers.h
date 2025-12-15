#ifndef CLASSMEMBERS_H
#define CLASSMEMBERS_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonArray>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class ClassMembers;
}

class ClassMembers : public QWidget
{
    Q_OBJECT

public:
    explicit ClassMembers(QWidget *parent = nullptr);
    ~ClassMembers();
    
    void setClassInfo(int classId, const QString &className);

signals:
    void backToClassDetail();

private slots:
    void on_backButton_clicked();
    void on_addStudentButton_clicked();
    void on_refreshButton_clicked();
    void handleRemoveStudent(int userId);

private:
    Ui::ClassMembers *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    QString currentClassName;
    
    void loadStudentsInClass();
    void loadStudentsNotInClass();
    void populateStudentList(const QJsonArray &students);
};

#endif // CLASSMEMBERS_H