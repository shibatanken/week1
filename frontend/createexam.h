#ifndef CREATEEXAM_H
#define CREATEEXAM_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QPushButton>

namespace Ui {
class CreateExam;
}

class CreateExam : public QWidget
{
    Q_OBJECT

public:
    explicit CreateExam(QWidget *parent = nullptr);
    ~CreateExam();
    
    void setClassId(int classId);
    void setExamId(int examId);
    void resetForm();

signals:
    void backToClassDetail();
    void examCreated();

private slots:
    void on_backButton_clicked();
    void on_saveExamButton_clicked();
    void on_addQuestionButton_clicked();
    void onAddAnswerButtonClicked();
    void onDeleteQuestionClicked();
    void onDeleteAnswerClicked();

private:
    Ui::CreateExam *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    int currentExamId;
    
    void loadExamQuestions();
    void createExam();
    void saveQuestion(int questionIndex);
};

#endif // CREATEEXAM_H