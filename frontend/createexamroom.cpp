#include "createexamroom.h"
#include "ui_createexamroom.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "config.h"

CreateExamRoom::CreateExamRoom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateExamRoom),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, this, &CreateExamRoom::onSaveButtonClicked);
    
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, &CreateExamRoom::validateInputs);
    connect(ui->descriptionLineEdit, &QLineEdit::textChanged, this, &CreateExamRoom::validateInputs);
    connect(ui->easySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &CreateExamRoom::validateInputs);
    connect(ui->mediumSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &CreateExamRoom::validateInputs);
    connect(ui->hardSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &CreateExamRoom::validateInputs);
    connect(ui->timeLimitSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &CreateExamRoom::validateInputs);
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &CreateExamRoom::validateInputs);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &CreateExamRoom::validateInputs);

    validateInputs();
}

CreateExamRoom::~CreateExamRoom()
{
    delete ui;
}

void CreateExamRoom::validateInputs()
{
    bool isValid = !ui->nameLineEdit->text().isEmpty() &&
                   !ui->descriptionLineEdit->text().isEmpty() &&
                   ui->easySpinBox->value() > 0 &&
                   ui->mediumSpinBox->value() > 0 &&
                   ui->hardSpinBox->value() > 0 &&
                   ui->timeLimitSpinBox->value() > 0 &&
                   ui->startDateTimeEdit->dateTime() < ui->endDateTimeEdit->dateTime();

    ui->saveButton->setEnabled(isValid);
}

void CreateExamRoom::onSaveButtonClicked()
{
    handleCreateExamRoom();
}

void CreateExamRoom::handleCreateExamRoom() {
    QJsonObject json;
    json["subject"] = ui->nameLineEdit->text();
    json["description"] = ui->descriptionLineEdit->text();
    json["number_of_easy_question"] = ui->easySpinBox->value();
    json["number_of_medium_question"] = ui->mediumSpinBox->value();
    json["number_of_hard_question"] = ui->hardSpinBox->value();
    json["time_limit"] = ui->timeLimitSpinBox->value();

    QDateTime startDateTime = ui->startDateTimeEdit->dateTime();
    QDateTime endDateTime = ui->endDateTimeEdit->dateTime();

    json["start"] = startDateTime.toString(Qt::ISODate);
    json["end"] = endDateTime.toString(Qt::ISODate);

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QString dataString = QString("CONTROL CREATE_ROOM\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        qDebug() << "Failed to connect to server";
        return;
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &CreateExamRoom::handleCreateExamRoomResponse);
}

void CreateExamRoom::handleCreateExamRoomResponse() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    qDebug() << "Create exam room response:" << responseString;
    
    if (responseString.startsWith("NOTIFICATION CREATE_ROOM_SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Phòng thi đã được tạo thành công!");
        emit roomCreated();
        
        QDialog *dialog = qobject_cast<QDialog *>(this->parentWidget());
        if (dialog) {
            dialog->close();
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể tạo phòng thi!");
    }
}