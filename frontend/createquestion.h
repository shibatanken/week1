#ifndef CREATEQUESTION_H
#define CREATEQUESTION_H

#include <QWidget>
#include <QTcpSocket>
#include <QButtonGroup>

namespace Ui {
class CreateQuestion;
}

class CreateQuestion : public QWidget
{
    Q_OBJECT

public:
    explicit CreateQuestion(QWidget *parent = nullptr);
    ~CreateQuestion();

public slots:
    void setClassId(int classId);
    void setRoomId(int roomId); // Compatibility wrapper

signals:
    void questionCreated();

private slots:
    void onSaveQuestionClicked();
    void updateSaveButtonState();
    void handleCreateQuestion();
    void handleCreateQuestionResponse();

private:
    Ui::CreateQuestion *ui;
    QTcpSocket *tcpSocket;
    QButtonGroup *radioButtonGroup;
    int classId = 0;
};

#endif // CREATEQUESTION_H
