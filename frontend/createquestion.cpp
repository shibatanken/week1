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

    answerListWidget = new QListWidget(ui->answerContainer);
    answerListWidget->setMinimumHeight(300);
    answerListWidget->setMinimumWidth(600);

    QVBoxLayout *containerLayout = new QVBoxLayout(ui->answerContainer);
    containerLayout->addWidget(answerListWidget);

    radioButtonGroup = new QButtonGroup(this);
    radioButtonGroup->setExclusive(true);

    connect(ui->addAnswerButton, &QPushButton::clicked, this, &CreateQuestion::onAddAnswerButtonClicked);
    
    ui->saveButton->setEnabled(false);
    connect(ui->questionTextEdit, &QTextEdit::textChanged, this, &CreateQuestion::updateSaveButtonState);
    connect(ui->saveButton, &QPushButton::clicked, this, &CreateQuestion::onSaveQuestionClicked);
}

CreateQuestion::~CreateQuestion()
{
    delete ui;
}

void CreateQuestion::setRoomId(int roomId) {
    qDebug() << "Set room ID:" << roomId;
    this->roomId = roomId;
}

void CreateQuestion::onAddAnswerButtonClicked() {
    QWidget *answerWidget = new QWidget();
    answerWidget->setMinimumHeight(80);
    answerWidget->setMinimumWidth(550);

    QPlainTextEdit *answerTextEdit = new QPlainTextEdit();
    answerTextEdit->setPlaceholderText("Nhập đáp án");
    answerTextEdit->setFixedHeight(60);
    answerTextEdit->setMinimumWidth(400);

    QRadioButton *correctAnswerRadioButton = new QRadioButton("Đúng");
    
    QPushButton *deleteButton = new QPushButton("X");
    deleteButton->setFixedSize(30, 30);
    deleteButton->setCursor(QCursor(Qt::PointingHandCursor));
    deleteButton->setStyleSheet("background-color: #ff4444; color: white;");

    radioButtonGroup->addButton(correctAnswerRadioButton);

    QHBoxLayout *answerRowLayout = new QHBoxLayout();
    answerRowLayout->addWidget(answerTextEdit);
    answerRowLayout->addWidget(correctAnswerRadioButton);
    answerRowLayout->addWidget(deleteButton);

    answerWidget->setLayout(answerRowLayout);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(answerWidget->sizeHint());
    answerListWidget->addItem(item);
    answerListWidget->setItemWidget(item, answerWidget);

    connect(deleteButton, &QPushButton::clicked, this, &CreateQuestion::onDeleteAnswerButtonClicked);
    connect(correctAnswerRadioButton, &QRadioButton::toggled, this, &CreateQuestion::updateSaveButtonState);

    updateSaveButtonState();
}

void CreateQuestion::onDeleteAnswerButtonClicked() {
    QPushButton *deleteButton = qobject_cast<QPushButton *>(sender());
    if (!deleteButton) return;

    for (int i = 0; i < answerListWidget->count(); ++i) {
        QListWidgetItem *item = answerListWidget->item(i);
        QWidget *answerWidget = answerListWidget->itemWidget(item);

        if (answerWidget && answerWidget->findChild<QPushButton *>() == deleteButton) {
            QRadioButton *radio = answerWidget->findChild<QRadioButton *>();
            if (radio) {
                radioButtonGroup->removeButton(radio);
            }
            delete answerListWidget->takeItem(i);
            break;
        }
    }
    updateSaveButtonState();
}

void CreateQuestion::updateSaveButtonState() {
    bool isQuestionEmpty = ui->questionTextEdit->toPlainText().trimmed().isEmpty();
    int answerCount = answerListWidget->count();

    bool hasCorrectAnswer = false;
    for (int i = 0; i < answerCount; ++i) {
        QListWidgetItem *item = answerListWidget->item(i);
        QWidget *answerWidget = answerListWidget->itemWidget(item);
        QRadioButton *radioButton = answerWidget->findChild<QRadioButton *>();
        if (radioButton && radioButton->isChecked()) {
            hasCorrectAnswer = true;
            break;
        }
    }

    bool canSave = !isQuestionEmpty && answerCount >= 2 && hasCorrectAnswer;
    ui->saveButton->setEnabled(canSave);
}

void CreateQuestion::onSaveQuestionClicked() {
    handleCreateQuestion();
}

void CreateQuestion::handleCreateQuestion() {
    QJsonObject json;
    json["room_id"] = roomId;
    json["content"] = ui->questionTextEdit->toPlainText();
    json["difficulty"] = ui->difficultyComboBox->currentIndex() + 1;

    QJsonArray answerList;
    for (int i = 0; i < answerListWidget->count(); ++i) {
        QListWidgetItem *item = answerListWidget->item(i);
        QWidget *answerWidget = answerListWidget->itemWidget(item);

        QPlainTextEdit *textEdit = answerWidget->findChild<QPlainTextEdit *>();
        QRadioButton *radioButton = answerWidget->findChild<QRadioButton *>();

        if (textEdit && radioButton) {
            QJsonObject answer;
            answer["content"] = textEdit->toPlainText();
            answer["is_true"] = radioButton->isChecked();
            answerList.append(answer);
        }
    }

    json["answer_list"] = answerList;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QString dataString = QString("CONTROL ADD_QUESTION\n%1").arg(QString(jsonData));

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        qDebug() << "Failed to connect to server";
        return;
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &CreateQuestion::handleCreateQuestionResponse);
}

void CreateQuestion::handleCreateQuestionResponse() {
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    qDebug() << "Create question response:" << responseString;

    if (responseString.startsWith("NOTIFICATION ADD_QUESTION_SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Câu hỏi đã được thêm vào ngân hàng đề!");
        emit questionCreated();
        
        QDialog *dialog = qobject_cast<QDialog *>(this->parentWidget());
        if (dialog) {
            dialog->close();
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể tạo câu hỏi!");
    }
}