#ifndef CREATEQUESTION_H
#define CREATEQUESTION_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    void setRoomId(int roomId);

signals:
    void questionCreated();

private slots:
    void onAddAnswerButtonClicked();
    void onDeleteAnswerButtonClicked();
    void onSaveQuestionClicked();
    void updateSaveButtonState();
    void handleCreateQuestion();
    void handleCreateQuestionResponse();

private:
    Ui::CreateQuestion *ui;
    QTcpSocket *tcpSocket;
    QListWidget *answerListWidget;
    QButtonGroup *radioButtonGroup;
    int roomId = 0;
};

#endif // CREATEQUESTION_H