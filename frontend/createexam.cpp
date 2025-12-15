#include "createexam.h"
#include "ui_createexam.h"
#include "config.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDebug>
#include <QScrollArea>
#include <QGroupBox>

CreateExam::CreateExam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateExam),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1),
    currentExamId(-1)
{
    ui->setupUi(this);
}

CreateExam::~CreateExam()
{
    delete ui;
}

void CreateExam::setClassId(int classId)
{
    currentClassId = classId;
    currentExamId = -1;
    resetForm();
}

void CreateExam::setExamId(int examId)
{
    currentExamId = examId;
    loadExamQuestions();
}

void CreateExam::resetForm()
{
    ui->examNameEdit->clear();
    ui->descriptionEdit->clear();
    ui->timeLimitSpinBox->setValue(60);
    ui->questionListWidget->clear();
}

void CreateExam::on_backButton_clicked()
{
    emit backToClassDetail();
}

void CreateExam::on_saveExamButton_clicked()
{
    QString examName = ui->examNameEdit->text().trimmed();
    if (examName.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập tên bài kiểm tra!");
        return;
    }
    
    createExam();
}

void CreateExam::createExam()
{
    QJsonObject json;
    json["exam_name"] = ui->examNameEdit->text().trimmed();
    json["description"] = ui->descriptionEdit->toPlainText().trimmed();
    json["class_id"] = currentClassId;
    json["time_limit"] = ui->timeLimitSpinBox->value();
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL CREATE_EXAM\n%1").arg(QString(jsonData));
    
    QTcpSocket *createSocket = new QTcpSocket(this);
    createSocket->connectToHost(IPADDRESS, 8081);
    
    if (createSocket->waitForConnected(3000)) {
        createSocket->write(dataString.toUtf8());
        createSocket->flush();
        
        if (createSocket->waitForReadyRead(3000)) {
            QByteArray response = createSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("CREATE_EXAM_SUCCESS")) {
                // Parse exam_id từ response
                int jsonStartIndex = responseString.indexOf('{');
                if (jsonStartIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject respObj = respDoc.object();
                    currentExamId = respObj["exam_id"].toInt();
                }
                
                QMessageBox::information(this, "Thành công", "Đã tạo bài kiểm tra! Bây giờ bạn có thể thêm câu hỏi.");
                ui->addQuestionButton->setEnabled(true);
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể tạo bài kiểm tra!");
            }
        }
    }
    
    createSocket->close();
    delete createSocket;
}

void CreateExam::on_addQuestionButton_clicked()
{
    if (currentExamId <= 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng lưu bài kiểm tra trước khi thêm câu hỏi!");
        return;
    }
    
    // Tạo widget cho câu hỏi
    QListWidgetItem *item = new QListWidgetItem(ui->questionListWidget);
    QWidget *questionWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(questionWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Header với nút xóa
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *questionLabel = new QLabel(QString("Câu hỏi %1:").arg(ui->questionListWidget->count()));
    questionLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    headerLayout->addWidget(questionLabel);
    headerLayout->addStretch();
    
    // Độ khó
    QComboBox *difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Dễ", 1);
    difficultyCombo->addItem("Trung bình", 2);
    difficultyCombo->addItem("Khó", 3);
    difficultyCombo->setObjectName("difficultyCombo");
    headerLayout->addWidget(new QLabel("Độ khó:"));
    headerLayout->addWidget(difficultyCombo);
    
    QPushButton *deleteQuestionBtn = new QPushButton("🗑 Xóa câu hỏi");
    deleteQuestionBtn->setStyleSheet("background-color: #dc3545; color: white; padding: 5px 10px;");
    deleteQuestionBtn->setCursor(Qt::PointingHandCursor);
    connect(deleteQuestionBtn, &QPushButton::clicked, this, &CreateExam::onDeleteQuestionClicked);
    headerLayout->addWidget(deleteQuestionBtn);
    
    mainLayout->addLayout(headerLayout);
    
    // Nội dung câu hỏi
    QPlainTextEdit *contentEdit = new QPlainTextEdit();
    contentEdit->setPlaceholderText("Nhập nội dung câu hỏi...");
    contentEdit->setMaximumHeight(80);
    contentEdit->setObjectName("contentEdit");
    mainLayout->addWidget(contentEdit);
    
    // Danh sách đáp án
    QLabel *answerLabel = new QLabel("Đáp án:");
    answerLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(answerLabel);
    
    QListWidget *answerList = new QListWidget();
    answerList->setMaximumHeight(150);
    answerList->setObjectName("answerList");
    mainLayout->addWidget(answerList);
    
    // Nút thêm đáp án
    QPushButton *addAnswerBtn = new QPushButton("➕ Thêm đáp án");
    addAnswerBtn->setStyleSheet("background-color: #17a2b8; color: white; padding: 5px 15px;");
    addAnswerBtn->setCursor(Qt::PointingHandCursor);
    connect(addAnswerBtn, &QPushButton::clicked, this, &CreateExam::onAddAnswerButtonClicked);
    mainLayout->addWidget(addAnswerBtn);
    
    // Nút lưu câu hỏi
    QPushButton *saveQuestionBtn = new QPushButton("💾 Lưu câu hỏi");
    saveQuestionBtn->setStyleSheet("background-color: #28a745; color: white; padding: 8px 20px;");
    saveQuestionBtn->setCursor(Qt::PointingHandCursor);
    saveQuestionBtn->setObjectName("saveQuestionBtn");
    int questionIndex = ui->questionListWidget->count() - 1;
    connect(saveQuestionBtn, &QPushButton::clicked, [this, questionIndex]() {
        saveQuestion(questionIndex);
    });
    mainLayout->addWidget(saveQuestionBtn);
    
    questionWidget->setLayout(mainLayout);
    questionWidget->setStyleSheet("background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px;");
    
    item->setSizeHint(QSize(0, 350));
    ui->questionListWidget->addItem(item);
    ui->questionListWidget->setItemWidget(item, questionWidget);
}

void CreateExam::onAddAnswerButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QWidget *questionWidget = btn->parentWidget();
    QListWidget *answerList = questionWidget->findChild<QListWidget*>("answerList");
    if (!answerList) return;
    
    QListWidgetItem *item = new QListWidgetItem(answerList);
    QWidget *answerWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(answerWidget);
    layout->setContentsMargins(5, 5, 5, 5);
    
    QLineEdit *answerEdit = new QLineEdit();
    answerEdit->setPlaceholderText("Nhập đáp án...");
    answerEdit->setObjectName("answerEdit");
    layout->addWidget(answerEdit);
    
    QRadioButton *correctRadio = new QRadioButton("Đáp án đúng");
    correctRadio->setObjectName("correctRadio");
    layout->addWidget(correctRadio);
    
    QPushButton *deleteBtn = new QPushButton("X");
    deleteBtn->setFixedSize(25, 25);
    deleteBtn->setStyleSheet("background-color: #dc3545; color: white;");
    deleteBtn->setCursor(Qt::PointingHandCursor);
    connect(deleteBtn, &QPushButton::clicked, this, &CreateExam::onDeleteAnswerClicked);
    layout->addWidget(deleteBtn);
    
    answerWidget->setLayout(layout);
    item->setSizeHint(QSize(0, 40));
    answerList->addItem(item);
    answerList->setItemWidget(item, answerWidget);
}

void CreateExam::onDeleteQuestionClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QWidget *questionWidget = btn->parentWidget()->parentWidget();
    
    for (int i = 0; i < ui->questionListWidget->count(); i++) {
        QListWidgetItem *item = ui->questionListWidget->item(i);
        if (ui->questionListWidget->itemWidget(item) == questionWidget) {
            delete ui->questionListWidget->takeItem(i);
            break;
        }
    }
}

void CreateExam::onDeleteAnswerClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QWidget *answerWidget = btn->parentWidget();
    QListWidget *answerList = qobject_cast<QListWidget*>(answerWidget->parentWidget()->parentWidget());
    if (!answerList) return;
    
    for (int i = 0; i < answerList->count(); i++) {
        QListWidgetItem *item = answerList->item(i);
        if (answerList->itemWidget(item) == answerWidget) {
            delete answerList->takeItem(i);
            break;
        }
    }
}

void CreateExam::saveQuestion(int questionIndex)
{
    if (questionIndex < 0 || questionIndex >= ui->questionListWidget->count()) return;
    
    QListWidgetItem *item = ui->questionListWidget->item(questionIndex);
    QWidget *questionWidget = ui->questionListWidget->itemWidget(item);
    
    // Lấy nội dung câu hỏi
    QPlainTextEdit *contentEdit = questionWidget->findChild<QPlainTextEdit*>("contentEdit");
    QComboBox *difficultyCombo = questionWidget->findChild<QComboBox*>("difficultyCombo");
    QListWidget *answerList = questionWidget->findChild<QListWidget*>("answerList");
    
    if (!contentEdit || !difficultyCombo || !answerList) return;
    
    QString content = contentEdit->toPlainText().trimmed();
    if (content.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập nội dung câu hỏi!");
        return;
    }
    
    if (answerList->count() < 2) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng thêm ít nhất 2 đáp án!");
        return;
    }
    
    // Tạo JSON cho câu hỏi
    QJsonObject json;
    json["exam_id"] = currentExamId;
    json["content"] = content;
    json["difficulty"] = difficultyCombo->currentData().toInt();
    
    // Tạo danh sách đáp án
    QJsonArray answersArray;
    bool hasCorrectAnswer = false;
    
    for (int i = 0; i < answerList->count(); i++) {
        QWidget *answerWidget = answerList->itemWidget(answerList->item(i));
        QLineEdit *answerEdit = answerWidget->findChild<QLineEdit*>("answerEdit");
        QRadioButton *correctRadio = answerWidget->findChild<QRadioButton*>("correctRadio");
        
        if (answerEdit && correctRadio) {
            QString answerContent = answerEdit->text().trimmed();
            if (!answerContent.isEmpty()) {
                QJsonObject answerObj;
                answerObj["content"] = answerContent;
                answerObj["is_correct"] = correctRadio->isChecked();
                answersArray.append(answerObj);
                
                if (correctRadio->isChecked()) hasCorrectAnswer = true;
            }
        }
    }
    
    if (!hasCorrectAnswer) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn ít nhất 1 đáp án đúng!");
        return;
    }
    
    json["answer_list"] = QString(QJsonDocument(answersArray).toJson(QJsonDocument::Compact));
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL ADD_EXAM_QUESTION\n%1").arg(QString(jsonData));
    
    QTcpSocket *saveSocket = new QTcpSocket(this);
    saveSocket->connectToHost(IPADDRESS, 8081);
    
    if (saveSocket->waitForConnected(3000)) {
        saveSocket->write(dataString.toUtf8());
        saveSocket->flush();
        
        if (saveSocket->waitForReadyRead(3000)) {
            QByteArray response = saveSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("ADD_EXAM_QUESTION_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Đã lưu câu hỏi!");
                
                // Disable nút lưu sau khi đã lưu
                QPushButton *saveBtn = questionWidget->findChild<QPushButton*>("saveQuestionBtn");
                if (saveBtn) {
                    saveBtn->setEnabled(false);
                    saveBtn->setText("✓ Đã lưu");
                    saveBtn->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 20px;");
                }
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể lưu câu hỏi!");
            }
        }
    }
    
    saveSocket->close();
    delete saveSocket;
}

void CreateExam::loadExamQuestions()
{
    // Để load câu hỏi khi edit exam (nếu cần)
}