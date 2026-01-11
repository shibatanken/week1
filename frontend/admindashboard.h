#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonArray>

namespace Ui {
class AdminDashboard;
}

class AdminDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    ~AdminDashboard();

    void loadData();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void logout();

private slots:
    void onReadyRead();
    void onApproveUser(int userId);
    void onRejectUser(int userId);
    void onDeleteUser(int userId);
    void onAddUser();
    void onTabChanged(int index);

private:
    Ui::AdminDashboard *ui;
    QTcpSocket *tcpSocket;
    QJsonArray pendingUsers;
    QJsonArray allUsers;
    bool isFirstShow;

    void loadPendingUsers();
    void loadAllUsers();
    void displayPendingUsers();
    void displayAllUsers();
};

#endif // ADMINDASHBOARD_H

