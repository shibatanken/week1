#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include "userdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Home; }
QT_END_NAMESPACE

class Home : public QWidget {
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();
    void updateUserInfo();

signals:
    void showExamRoomManagement();
    void logout();

private slots:
    void on_examRoomButton_clicked();
    void on_logoutButton_clicked();

private:
    Ui::Home *ui;
};

#endif // HOME_H