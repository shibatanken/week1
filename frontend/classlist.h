#ifndef CLASSLIST_H
#define CLASSLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class ClassList;
}

class ClassList : public QWidget
{
    Q_OBJECT

public:
    explicit ClassList(QWidget *parent = nullptr);
    ~ClassList();

signals:
    void showSignin();
    void showClassDetail(int classId);

private slots:
    void on_createClassButton_clicked();
    void on_refreshButton_clicked();
    void onReadyRead();
    void handleClassItemClicked(int classId);

private:
    Ui::ClassList *ui;
    QTcpSocket *tcpSocket;
    
    void loadClassList();
    void populateClassList(const QJsonArray &classes);
    void createClassItem(const QJsonObject &classObj);
};

#endif // CLASSLIST_H

