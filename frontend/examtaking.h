#ifndef EXAMTAKING_H
#define EXAMTAKING_H

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QButtonGroup>

namespace Ui {
class ExamTaking;
}

class ExamTaking : public QWidget
{
    Q_OBJECT

public:
    explicit ExamTaking(QWidget *parent = nullptr);
    ~ExamTaking();
    
    void setExamInfo(int examId, QString examName, int timeLimit);
    void startExam();

signals:
    void backToExamList();
    void examFinished(int submissionId);

private slots:
    void onReadyRead();
    void updateTimer();
    void onPreviousClicked();
    void onNextClicked();
    void onSubmitClicked();
    void onAnswerSelected(int questionIndex, QString answer);
    void onBackToListClicked();
    void onAppealClicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ExamTaking *ui;
    QTcpSocket *tcpSocket;
    QTimer *examTimer;
    
    int examId;
    int submissionId;
    QString examName;
    int timeLimit; // in minutes
    int remainingSeconds;
    
    QJsonArray questions;
    int currentQuestionIndex;
    QMap<int, QString> userAnswers; // question_id -> answer
    QJsonArray resultAnswers; // Store detailed answers for appeal dialog
    
    void loadQuestions();
    void displayQuestion(int index);
    void saveCurrentAnswer();
    void saveAllAnswers();
    void submitExam();
    void showResult(const QJsonObject &result);
};

#endif // EXAMTAKING_H

