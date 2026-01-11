#include "appealdialog.h"
#include "ui_appealdialog.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTcpSocket>
#include <QDebug>

AppealDialog::AppealDialog(int submissionId, int examId, const QJsonArray &resultAnswers, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppealDialog),
    submissionId(submissionId),
    examId(examId),
    resultAnswers(resultAnswers)
{
    qDebug() << "=== AppealDialog constructor ===";
    qDebug() << "submissionId:" << submissionId;
    qDebug() << "examId:" << examId;
    qDebug() << "resultAnswers.size():" << resultAnswers.size();
    
    ui->setupUi(this);
    setWindowTitle("Khiếu nại kết quả");
    setModal(true);
    setWindowFlags(windowFlags() | Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    
    // Populate question combo box
    for (int i = 0; i < resultAnswers.size(); i++) {
        QJsonObject a = resultAnswers[i].toObject();
        int questionId = a["question_id"].toInt();
        QString content = a["content"].toString();
        QString displayText = QString("Câu %1: %2").arg(i + 1).arg(content.left(50) + "...");
        ui->cmbQuestion->addItem(displayText, questionId);
    }
    
    qDebug() << "Added" << resultAnswers.size() << "questions to combo box";
    
    connect(ui->btnSubmit, &QPushButton::clicked, this, &AppealDialog::onSubmitClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AppealDialog::onCancelClicked);
    
    qDebug() << "AppealDialog setup complete";
}

AppealDialog::~AppealDialog()
{
    delete ui;
}

void AppealDialog::onSubmitClicked()
{
    int selectedIndex = ui->cmbQuestion->currentIndex();
    if (selectedIndex < 0 || selectedIndex >= resultAnswers.size()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn câu hỏi cần khiếu nại!");
        return;
    }
    
    QString reason = ui->txtReason->toPlainText().trimmed();
    if (reason.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập lý do khiếu nại!");
        return;
    }
    
    QJsonObject selectedAnswer = resultAnswers[selectedIndex].toObject();
    int questionId = selectedAnswer["question_id"].toInt();
    
    // Send appeal to server
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, PORT);
    
    if (socket->waitForConnected(3000)) {
        QJsonObject json;
        json["submission_id"] = submissionId;
        json["question_id"] = questionId;
        json["user_id"] = UserData::instance().getUserId();
        json["reason"] = reason;
        
        qDebug() << "Submitting appeal with:" << QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
        
        QString request = QString("CONTROL SUBMIT_APPEAL\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        socket->write(request.toUtf8());
        socket->flush();
        
        if (socket->waitForReadyRead(3000)) {
            QByteArray data = socket->readAll();
            QString response = QString::fromUtf8(data);
            
            qDebug() << "Appeal response:" << response;
            
            if (response.contains("SUBMIT_APPEAL_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Khiếu nại đã được gửi thành công!");
                accept();
            } else {
                qDebug() << "Appeal submission failed. Response:" << response;
                QMessageBox::critical(this, "Lỗi", QString("Không thể gửi khiếu nại.\n\nPhản hồi từ server: %1").arg(response.left(200)));
            }
        } else {
            QMessageBox::critical(this, "Lỗi", "Không nhận được phản hồi từ server!");
        }
        
        socket->close();
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối đến server!");
    }
    
    socket->deleteLater();
}

void AppealDialog::onCancelClicked()
{
    reject();
}

