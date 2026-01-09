#ifndef APPEALMANAGER_H
#define APPEALMANAGER_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonArray>

namespace Ui {
class AppealManager;
}

class AppealManager : public QWidget
{
    Q_OBJECT

public:
    explicit AppealManager(QWidget *parent = nullptr);
    ~AppealManager();
    
    void loadAppeals();
    void setMode(bool isTeacher);

signals:
    void backPressed();

private slots:
    void onReadyRead();
    void onSubmitAppeal();
    void onReviewAppeal(int appealId, QString status);

private:
    Ui::AppealManager *ui;
    QTcpSocket *tcpSocket;
    QJsonArray appeals;
    bool isTeacherMode;
    
    void displayAppeals();
    void displayAppealForm();
};

#endif // APPEALMANAGER_H

