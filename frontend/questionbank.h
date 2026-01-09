#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include <QDialog>
#include <QTcpSocket>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QJsonObject>

class QuestionBank : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionBank(int classId, bool selectionMode = false, QWidget *parent = nullptr);
    ~QuestionBank();

    int getSelectedQuestionId() const;

signals:
    void questionSelected(int questionId);

private slots:
    void loadQuestions();
    void onAddQuestionClicked();
    void onDeleteQuestionClicked();
    void onSelectQuestionClicked();

private:
    int m_classId;
    bool m_selectionMode;
    int m_selectedQuestionId;
    
    QListWidget *m_listWidget;
    QPushButton *m_addBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_selectBtn;     // Used in selection mode
    QPushButton *m_refreshBtn;
    
    void setupUi();
    void showAddQuestionDialog();
};

#endif
