#ifndef EXAMROOMMANAGEMENT_H
#define EXAMROOMMANAGEMENT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class ExamRoomManagement;
}

class ExamRoomManagement : public QWidget
{
    Q_OBJECT

public:
    explicit ExamRoomManagement(QWidget *parent = nullptr);
    ~ExamRoomManagement();

signals:
    void logout();
    void showHome();

private slots:
    void onCreateExamRoomClicked();
    void onViewRoomClicked(int row);
    void handleGetExamRoomList();
    void handleGetExamRoomListResponse();
    void onLogoutClicked();

private:
    Ui::ExamRoomManagement *ui;
    QTcpSocket *tcpSocket;
    void refreshRoomList();
};

#endif // EXAMROOMMANAGEMENT_H