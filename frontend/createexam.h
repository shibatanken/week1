#ifndef CREATEEXAM_H
#define CREATEEXAM_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
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
    void setExamId(int examId); // For edit mode (optional)

signals:
    void backToClassDetail();

private slots:
    void on_backButton_clicked();
    void on_saveExamButton_clicked();
    void on_addQuestionButton_clicked();
    void on_publishButton_clicked();
    void onAddAnswerButtonClicked();
    void onDeleteQuestionClicked();
    void onDeleteAnswerClicked();

private:
    Ui::CreateExam *ui;
    QTcpSocket *tcpSocket;
    int currentClassId;
    int currentExamId;
    QPushButton *importBankBtn;
    
    void resetForm();
    void createExam();
    void saveQuestion(int questionIndex);
    void loadExamQuestions();
    void loadExamInfo();
    void importQuestion(int questionId);
    void deleteQuestionFromExam(int questionId);
    void publishExam();
    
    QString currentExamStatus;
};

#endif // CREATEEXAM_H
