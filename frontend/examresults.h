#ifndef EXAMRESULTS_H
#define EXAMRESULTS_H

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ExamResults; }
QT_END_NAMESPACE

class ExamResults : public QWidget {
    Q_OBJECT

public:
    explicit ExamResults(QWidget *parent = nullptr);
    ~ExamResults();
    void loadResults(int examId, int userId);

signals:
    void backToExamList();

private slots:
    void on_backButton_clicked();
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketError();

private:
    Ui::ExamResults *ui;
    QTcpSocket *socket;
    int currentExamId;
    int currentUserId;

    void connectToServer();
    void requestResults();
    void displayResults(const QString &jsonData);
};

#endif // EXAMRESULTS_H
