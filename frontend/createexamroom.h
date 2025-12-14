#ifndef CREATEEXAMROOM_H
#define CREATEEXAMROOM_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class CreateExamRoom;
}

class CreateExamRoom : public QWidget
{
    Q_OBJECT

public:
    explicit CreateExamRoom(QWidget *parent = nullptr);
    ~CreateExamRoom();

signals:
    void roomCreated();

private slots:
    void onSaveButtonClicked();
    void validateInputs();
    void handleCreateExamRoom();
    void handleCreateExamRoomResponse();

private:
    Ui::CreateExamRoom *ui;
    QTcpSocket *tcpSocket;
};

#endif // CREATEEXAMROOM_H