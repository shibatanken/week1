#include "classdetail.h"
#include "ui_classdetail.h"
#include "userdata.h"
#include "config.h"
#include "questionbank.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QDebug>

ClassDetail::ClassDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassDetail),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1)
{
    ui->setupUi(this);
    
    questionBankBtn = new QPushButton("Ngân hàng câu hỏi", this);
    questionBankBtn->setCursor(Qt::PointingHandCursor);
    ui->headerLayout->insertWidget(2, questionBankBtn);
    
    connect(questionBankBtn, &QPushButton::clicked, [this](){
        if(currentClassId != -1) {
            QuestionBank dlg(currentClassId, false, this);
            dlg.exec();
        }
    });

    connect(ui->examListWidget, &QListWidget::itemClicked, this, &ClassDetail::onExamItemClicked);
}

ClassDetail::~ClassDetail() { delete ui; }

void ClassDetail::setClassId(int classId)
{
    currentClassId = classId;
    QString userRole = UserData::instance().getRole();
    if (userRole == "student") {
        ui->createExamButton->hide();
        ui->membersButton->hide();
        questionBankBtn->hide();
    } else {
        ui->createExamButton->show();
        ui->membersButton->show();
        questionBankBtn->show();
    }
    loadClassDetail();
    loadExamsInClass();
}

void ClassDetail::loadClassDetail()
{   
    QJsonObject json; json["class_id"] = currentClassId;
    QJsonDocument doc(json);
    QString dataString = QString("CONTROL GET_CLASS_DETAIL\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    QTcpSocket *detailSocket = new QTcpSocket(this);
    detailSocket->connectToHost(IPADDRESS, 8081);
    
    if (detailSocket->waitForConnected(3000)) {
        detailSocket->write(dataString.toUtf8());
        detailSocket->flush();
        if (detailSocket->waitForReadyRead(3000)) {
            QString responseString(detailSocket->readAll());
            if (responseString.startsWith("DATA JSON GET_CLASS_DETAIL")) {
                int idx = responseString.indexOf('{');
                if (idx != -1) {
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseString.mid(idx).toUtf8());
                    QJsonObject classObj = jsonDoc.object();
                    currentClassName = classObj["class_name"].toString();
                    ui->classNameLabel->setText(currentClassName);
                    ui->descriptionLabel->setText(classObj["description"].toString());
                    ui->teacherLabel->setText("Giáo viên: " + classObj["teacher_name"].toString());
                }
            }
        }
    }
    detailSocket->close(); delete detailSocket;
}

void ClassDetail::loadExamsInClass()
{
    ui->examListWidget->clear();
    QJsonObject json; json["class_id"] = currentClassId;
    QJsonDocument doc(json);
    QString dataString = QString("CONTROL GET_EXAMS_IN_CLASS\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    QTcpSocket *examSocket = new QTcpSocket(this);
    examSocket->connectToHost(IPADDRESS, 8081);
    
    if (examSocket->waitForConnected(3000)) {
        examSocket->write(dataString.toUtf8());
        examSocket->flush();
        if (examSocket->waitForReadyRead(3000)) {
            QString responseString(examSocket->readAll());
            if (responseString.startsWith("DATA JSON GET_EXAMS_IN_CLASS")) {
                int idx = responseString.indexOf('{');
                if (idx != -1) {
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseString.mid(idx).toUtf8());
                    QJsonObject rootObj = jsonDoc.object();
                    QJsonArray examsArray = rootObj["exams"].toArray();
                    for (const QJsonValue &value : examsArray) {
                        QJsonObject examObj = value.toObject();
                        int examId = examObj["id"].toInt();
                        QString examTitle = examObj["title"].toString();
                        QString status = examObj["status"].toString();
                        QString displayText = QString("%1 - %2").arg(examTitle).arg(status);
                        QListWidgetItem *item = new QListWidgetItem(displayText);
                        item->setData(Qt::UserRole, examId);
                        if (status == "closed") item->setForeground(Qt::gray);
                        else { item->setForeground(Qt::black); item->setFont(QFont("Arial", 12, QFont::Bold)); }
                        ui->examListWidget->addItem(item);
                    }
                }
            }
        }
    }
    examSocket->close(); delete examSocket;
}

void ClassDetail::on_backButton_clicked() { emit backToClassList(); }
void ClassDetail::on_createExamButton_clicked() { emit openCreateExam(currentClassId); }
void ClassDetail::on_membersButton_clicked() { emit openClassMembers(currentClassId, currentClassName); }
void ClassDetail::onExamItemClicked(QListWidgetItem *item) { int examId = item->data(Qt::UserRole).toInt(); emit openExamDetail(examId); }
void ClassDetail::on_refreshButton_clicked() { loadExamsInClass(); }
