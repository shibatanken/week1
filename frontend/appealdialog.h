#ifndef APPEALDIALOG_H
#define APPEALDIALOG_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class AppealDialog;
}

class AppealDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppealDialog(int submissionId, int examId, const QJsonArray &resultAnswers, QWidget *parent = nullptr);
    ~AppealDialog();

private slots:
    void onSubmitClicked();
    void onCancelClicked();

private:
    Ui::AppealDialog *ui;
    int submissionId;
    int examId;
    QJsonArray resultAnswers;
};

#endif // APPEALDIALOG_H



