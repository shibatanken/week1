#ifndef CLASSLIST_H
#define CLASSLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class ClassList;
}

class ClassList : public QWidget
{
    Q_OBJECT

public:
    explicit ClassList(QWidget *parent = nullptr);
    ~ClassList();
    void loadClassList(); 

signals:
    void showSignin();
    void showClassDetail(int classId);
    void logout();
    void showAppealManager();
    void showAdminDashboard();

private slots:
    void on_createClassButton_clicked();
    void on_refreshButton_clicked();
    void on_logoutButton_clicked(); 
    void onReadyRead();
    void handleClassItemClicked(int classId);

private:
    Ui::ClassList *ui;
    QTcpSocket *tcpSocket;
    void updateUIByRole(); 
    void populateClassList(const QJsonArray &classes);
    void createClassItem(const QJsonObject &classObj);
};

#endif // CLASSLIST_H
