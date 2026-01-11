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
    void setSubmissionId(int submissionId);

signals:
    void backPressed();

private slots:
    void onReadyRead();
    void onSubmitAppeal();
    void onReviewAppeal(int appealId, QString status, double currentScore, QString examName);

private:
    Ui::AppealManager *ui;
    QTcpSocket *tcpSocket;
    QJsonArray appeals;
    bool isTeacherMode;
    int currentSubmissionId;
    
    void displayAppeals();
    void displayAppealForm();
};

#endif // APPEALMANAGER_H

