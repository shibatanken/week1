#ifndef EXAMSCORES_H
#define EXAMSCORES_H

#include <QWidget>
#include <QTcpSocket>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ExamScores; }
QT_END_NAMESPACE

class ExamScores : public QWidget {
    Q_OBJECT

public:
    explicit ExamScores(QWidget *parent = nullptr);
    ~ExamScores();
    void loadScores(int examId);

signals:
    void backToExamList();

private slots:
    void on_backButton_clicked();
    void on_refreshButton_clicked();
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketError();

private:
    Ui::ExamScores *ui;
    QTcpSocket *socket;
    int currentExamId;

    void connectToServer();
    void requestScores();
    void displayScores(const QString &jsonData);
};

#endif // EXAMSCORES_H
