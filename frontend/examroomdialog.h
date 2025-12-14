#ifndef EXAMROOMDIALOG_H
#define EXAMROOMDIALOG_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class ExamRoomDialog;
}

class ExamRoomDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ExamRoomDialog(QWidget *parent = nullptr);
    ~ExamRoomDialog();
    void setRoomDetails(int roomId, const QString &roomName, const QString &description, const QString &timeLimit);

signals:
    void roomUpdated();

private slots:
    void onAddQuestionClicked();
    void handleGetQuestionBank();
    void handleGetQuestionBankResponse();

private:
    Ui::ExamRoomDialog *ui;
    QTcpSocket *tcpSocket;
    int examRoomId;
};

#endif // EXAMROOMDIALOG_H