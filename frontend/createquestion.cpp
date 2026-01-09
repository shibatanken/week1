#include "createquestion.h"
#include "ui_createquestion.h"

#include <QDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include "config.h"

CreateQuestion::CreateQuestion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateQuestion),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // Group radio buttons
    radioButtonGroup = new QButtonGroup(this);
    radioButtonGroup->addButton(ui->radioA, 0);
    radioButtonGroup->addButton(ui->radioB, 1);
    radioButtonGroup->addButton(ui->radioC, 2);
    radioButtonGroup->addButton(ui->radioD, 3);
    
    ui->saveButton->setEnabled(false);

    // Connect signals for validation
    connect(ui->questionTextEdit, &QTextEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    connect(ui->optionA, &QLineEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    connect(ui->optionB, &QLineEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    connect(ui->optionC, &QLineEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    connect(ui->optionD, &QLineEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    
    // Also update if radio button selection changes (though one is always checked by default)
    connect(radioButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &CreateQuestion::updateSaveButtonState);

    connect(ui->saveButton, &QPushButton::clicked, this, &CreateQuestion::onSaveQuestionClicked);
}

CreateQuestion::~CreateQuestion()
{
    delete ui;
}

void CreateQuestion::setClassId(int classId) {
    qDebug() << "Set class ID:" << classId;
    this->classId = classId;
}

void CreateQuestion::setRoomId(int roomId) {
    setClassId(roomId);
}

void CreateQuestion::updateSaveButtonState() {
    bool isQuestionEmpty = ui->questionTextEdit->toPlainText().trimmed().isEmpty();
    bool isOptAEmpty = ui->optionA->text().trimmed().isEmpty();
    bool isOptBEmpty = ui->optionB->text().trimmed().isEmpty();
    bool isOptCEmpty = ui->optionC->text().trimmed().isEmpty();
    bool isOptDEmpty = ui->optionD->text().trimmed().isEmpty();

    bool hasCorrectAnswer = (radioButtonGroup->checkedButton() != nullptr);

    bool canSave = !isQuestionEmpty && !isOptAEmpty && !isOptBEmpty && !isOptCEmpty && !isOptDEmpty && hasCorrectAnswer;
    ui->saveButton->setEnabled(canSave);
}

void CreateQuestion::onSaveQuestionClicked() {
    handleCreateQuestion();
}

void CreateQuestion::handleCreateQuestion() {
    QJsonObject json;
    json["class_id"] = classId;
    json["content"] = ui->questionTextEdit->toPlainText();
    json["difficulty"] = ui->difficultyComboBox->currentIndex() + 1;

    json["option_a"] = ui->optionA->text();
    json["option_b"] = ui->optionB->text();
    json["option_c"] = ui->optionC->text();
    json["option_d"] = ui->optionD->text();
    
    QString correct = "A";
    int checkedId = radioButtonGroup->checkedId();
    if (checkedId == 1) correct = "B";
    else if (checkedId == 2) correct = "C";
    else if (checkedId == 3) correct = "D";
    
    json["correct_option"] = correct;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // Command ADD_EXAM_QUESTION
    QString dataString = QString("CONTROL ADD_EXAM_QUESTION\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        qDebug() << "Failed to connect to server";
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối đến server!");
        return;
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &CreateQuestion::handleCreateQuestionResponse);
}

void CreateQuestion::handleCreateQuestionResponse() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    qDebug() << "Create question response:" << responseString;

    if (responseString.startsWith("NOTIFICATION ADD_EXAM_QUESTION_SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Câu hỏi đã được thêm vào ngân hàng đề!");
        emit questionCreated();
        
        QDialog *dialog = qobject_cast<QDialog *>(this->parentWidget());
        if (dialog) {
            dialog->close();
        } else {
            this->close();
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể tạo câu hỏi! " + responseString);
    }
}
