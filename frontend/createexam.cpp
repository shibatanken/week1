#include "createexam.h"
#include "ui_createexam.h"
#include "config.h"
#include "questionbank.h"

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
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QTcpSocket>
#include <QTimer>
#include <QPlainTextEdit>

CreateExam::CreateExam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateExam),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1),
    currentExamId(-1)
{
    ui->setupUi(this);
    
    importBankBtn = new QPushButton("Nhập từ Ngân hàng", this);
    importBankBtn->setCursor(Qt::PointingHandCursor);
    importBankBtn->setEnabled(true);  // Enable ngay từ đầu
    ui->questionHeaderLayout->insertWidget(2, importBankBtn);
    
    connect(importBankBtn, &QPushButton::clicked, [this](){
         if(currentExamId <= 0) {
             QMessageBox::warning(this, "Lỗi", "Vui lòng lưu bài kiểm tra trước khi nhập câu hỏi");
             return;
         }
         if(currentClassId == -1) {
             QMessageBox::warning(this, "Lỗi", "Không xác định được lớp học");
             return;
         }
         QuestionBank dlg(currentClassId, true, this);
         if(dlg.exec() == QDialog::Accepted) {
             int qId = dlg.getSelectedQuestionId();
             if(qId > 0) {
                 importQuestion(qId);
             } else {
                 QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một câu hỏi");
             }
         }
    });

    // Disable add button initially
    ui->addQuestionButton->setEnabled(false);
    
    // Connect signals
    connect(ui->saveExamButton, &QPushButton::clicked, this, &CreateExam::on_saveExamButton_clicked);
    connect(ui->addQuestionButton, &QPushButton::clicked, this, &CreateExam::on_addQuestionButton_clicked);
    connect(ui->publishButton, &QPushButton::clicked, this, &CreateExam::on_publishButton_clicked);
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
    loadExamInfo();
    loadExamQuestions();
    ui->addQuestionButton->setEnabled(true);
    
    // Update publish button based on status
    if (currentExamStatus == "draft") {
        ui->publishButton->setEnabled(true);
        ui->publishButton->setText("🚀 Công bố đề thi");
    } else {
        ui->publishButton->setEnabled(false);
        ui->publishButton->setText("✅ Đã công bố");
    }
}

void CreateExam::resetForm()
{
    ui->examNameEdit->clear();
    ui->descriptionEdit->clear();
    ui->timeLimitSpinBox->setValue(60);
    ui->questionListWidget->clear();
    ui->addQuestionButton->setEnabled(false);
    ui->publishButton->setEnabled(false);
    ui->publishButton->setText("🚀 Công bố đề thi");
    currentExamStatus = "draft";
}

void CreateExam::loadExamInfo()
{
    if (currentExamId <= 0) return;
    
    QJsonObject json;
    json["exam_id"] = currentExamId;
    QJsonDocument doc(json);
    QString req = QString("CONTROL GET_EXAM_DETAIL\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, 8081);
    if (socket->waitForConnected(3000)) {
        socket->write(req.toUtf8());
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QString resp = QString(socket->readAll());
            qDebug() << "loadExamInfo response:" << resp.left(300);
            
            int idx = resp.indexOf('{');
            if (idx != -1) {
                QJsonDocument d = QJsonDocument::fromJson(resp.mid(idx).toUtf8());
                QJsonObject obj = d.object();
                
                // Có thể data nằm trong "data" hoặc trực tiếp
                if (obj.contains("data")) {
                    obj = obj["data"].toObject();
                }
                
                ui->examNameEdit->setText(obj["exam_name"].toString());
                ui->descriptionEdit->setText(obj["description"].toString());
                ui->timeLimitSpinBox->setValue(obj["time_limit"].toInt());
                currentExamStatus = obj["status"].toString();
                currentClassId = obj["class_id"].toInt();
                
                // Update title
                ui->titleLabel->setText(QString("📝 Chỉnh sửa đề thi: %1").arg(obj["exam_name"].toString()));
            }
        }
    }
    socket->close();
    socket->deleteLater();
}

void CreateExam::on_backButton_clicked()
{
    // Nếu đã tạo exam và có câu hỏi → Thông báo đã lưu
    if (currentExamId > 0) {
        int questionCount = ui->questionListWidget->count();
        if (questionCount > 0) {
            QMessageBox::information(this, "Thông báo", 
                QString("Đề thi đã được lưu với %1 câu hỏi.\n\n"
                        "Bạn có thể chỉnh sửa sau bằng cách vào lại đề thi.")
                .arg(questionCount));
        } else {
            // Có exam nhưng chưa có câu hỏi
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Xác nhận",
                "Đề thi chưa có câu hỏi nào. Bạn có muốn quay lại không?\n\n"
                "(Đề thi vẫn được lưu, bạn có thể thêm câu hỏi sau)",
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) return;
        }
    }
    
    // Reset form trước khi quay lại
    resetForm();
    currentExamId = -1;
    currentClassId = -1;
    emit backToClassDetail();
}

void CreateExam::on_saveExamButton_clicked()
{
    QString examName = ui->examNameEdit->text().trimmed();
    if (examName.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập tên bài kiểm tra!");
        return;
    }
    
    // Chỉ tạo exam nếu chưa có
    if (currentExamId <= 0) {
        createExam();
    } else {
        // Đã có exam rồi, không cần tạo lại
        QMessageBox::information(this, "Thông báo", "Đề thi đã được lưu. Bạn có thể thêm câu hỏi.");
    }
}

void CreateExam::on_publishButton_clicked()
{
    if (currentExamId <= 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng lưu đề thi trước khi công bố!");
        return;
    }
    
    int questionCount = ui->questionListWidget->count();
    if (questionCount == 0) {
        QMessageBox::warning(this, "Lỗi", "Đề thi phải có ít nhất 1 câu hỏi trước khi công bố!");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Xác nhận công bố",
        QString("Bạn có chắc muốn công bố đề thi này?\n\n"
                "Đề thi: %1\n"
                "Số câu hỏi: %2\n\n"
                "Sau khi công bố, sinh viên sẽ có thể tham gia làm bài.")
        .arg(ui->examNameEdit->text())
        .arg(questionCount),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        publishExam();
    }
}

void CreateExam::publishExam()
{
    QJsonObject json;
    json["exam_id"] = currentExamId;
    json["status"] = "published";
    QJsonDocument doc(json);
    QString req = QString("CONTROL UPDATE_EXAM_STATUS\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, 8081);
    if (socket->waitForConnected(3000)) {
        socket->write(req.toUtf8());
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QString resp = QString(socket->readAll());
            if (resp.contains("SUCCESS")) {
                QMessageBox::information(this, "Thành công", 
                    "Đề thi đã được công bố!\n\nSinh viên giờ có thể thấy và tham gia làm bài.");
                ui->publishButton->setEnabled(false);
                ui->publishButton->setText("✅ Đã công bố");
                currentExamStatus = "published";
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể công bố đề thi: " + resp);
            }
        }
    }
    socket->close();
    socket->deleteLater();
}

void CreateExam::createExam()
{
    QJsonObject json;
    json["exam_name"] = ui->examNameEdit->text().trimmed();
    json["description"] = ui->descriptionEdit->text().trimmed();
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
                int jsonStartIndex = responseString.indexOf('{');
                if (jsonStartIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject respObj = respDoc.object();
                    currentExamId = respObj["exam_id"].toInt();
                }
                
                QMessageBox::information(this, "Thành công", "Đã tạo bài kiểm tra! Bây giờ bạn có thể thêm câu hỏi.");
                ui->addQuestionButton->setEnabled(true);
                ui->publishButton->setEnabled(true);
                importBankBtn->setEnabled(true);
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
    
    QListWidgetItem *item = new QListWidgetItem(ui->questionListWidget);
    QWidget *questionWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(questionWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *questionLabel = new QLabel(QString("Câu hỏi %1:").arg(ui->questionListWidget->count()));
    questionLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    headerLayout->addWidget(questionLabel);
    headerLayout->addStretch();
    
    QPushButton *deleteQuestionBtn = new QPushButton("🗑 Xóa câu hỏi");
    deleteQuestionBtn->setStyleSheet("background-color: #dc3545; color: white; padding: 5px 10px;");
    deleteQuestionBtn->setCursor(Qt::PointingHandCursor);
    connect(deleteQuestionBtn, &QPushButton::clicked, this, &CreateExam::onDeleteQuestionClicked);
    headerLayout->addWidget(deleteQuestionBtn);
    
    mainLayout->addLayout(headerLayout);
    
    QPlainTextEdit *contentEdit = new QPlainTextEdit();
    contentEdit->setPlaceholderText("Nhập nội dung câu hỏi...");
    contentEdit->setMaximumHeight(80);
    contentEdit->setObjectName("contentEdit");
    mainLayout->addWidget(contentEdit);
    
    QLabel *answerLabel = new QLabel("Nhập 4 phương án và chọn một đáp án đúng:");
    answerLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(answerLabel);

    QGridLayout *optionsLayout = new QGridLayout();
    optionsLayout->setSpacing(10);
    QButtonGroup *radioGroup = new QButtonGroup(questionWidget);
    radioGroup->setObjectName("radioGroup");

    // Row 0: A
    QLabel *labelA = new QLabel("A.");
    labelA->setMinimumWidth(30);
    optionsLayout->addWidget(labelA, 0, 0);
    QLineEdit *optionA = new QLineEdit();
    optionA->setPlaceholderText("Nhập đáp án A");
    optionA->setObjectName("optionA");
    optionA->setMinimumWidth(300);
    optionsLayout->addWidget(optionA, 0, 1);
    QRadioButton *checkA = new QRadioButton("Đúng");
    checkA->setObjectName("checkA");
    radioGroup->addButton(checkA);
    optionsLayout->addWidget(checkA, 0, 2);

    // Row 1: B
    QLabel *labelB = new QLabel("B.");
    labelB->setMinimumWidth(30);
    optionsLayout->addWidget(labelB, 1, 0);
    QLineEdit *optionB = new QLineEdit();
    optionB->setPlaceholderText("Nhập đáp án B");
    optionB->setObjectName("optionB");
    optionB->setMinimumWidth(300);
    optionsLayout->addWidget(optionB, 1, 1);
    QRadioButton *checkB = new QRadioButton("Đúng");
    checkB->setObjectName("checkB");
    radioGroup->addButton(checkB);
    optionsLayout->addWidget(checkB, 1, 2);

    // Row 2: C
    QLabel *labelC = new QLabel("C.");
    labelC->setMinimumWidth(30);
    optionsLayout->addWidget(labelC, 2, 0);
    QLineEdit *optionC = new QLineEdit();
    optionC->setPlaceholderText("Nhập đáp án C");
    optionC->setObjectName("optionC");
    optionC->setMinimumWidth(300);
    optionsLayout->addWidget(optionC, 2, 1);
    QRadioButton *checkC = new QRadioButton("Đúng");
    checkC->setObjectName("checkC");
    radioGroup->addButton(checkC);
    optionsLayout->addWidget(checkC, 2, 2);

    // Row 3: D
    QLabel *labelD = new QLabel("D.");
    labelD->setMinimumWidth(30);
    optionsLayout->addWidget(labelD, 3, 0);
    QLineEdit *optionD = new QLineEdit();
    optionD->setPlaceholderText("Nhập đáp án D");
    optionD->setObjectName("optionD");
    optionD->setMinimumWidth(300);
    optionsLayout->addWidget(optionD, 3, 1);
    QRadioButton *checkD = new QRadioButton("Đúng");
    checkD->setObjectName("checkD");
    radioGroup->addButton(checkD);
    optionsLayout->addWidget(checkD, 3, 2);

    mainLayout->addLayout(optionsLayout);
    
    QPushButton *saveQuestionBtn = new QPushButton("💾 Lưu câu hỏi");
    saveQuestionBtn->setStyleSheet("background-color: #28a745; color: white; padding: 8px 20px; margin-top: 10px;");
    saveQuestionBtn->setCursor(Qt::PointingHandCursor);
    saveQuestionBtn->setObjectName("saveQuestionBtn");
    
    connect(saveQuestionBtn, &QPushButton::clicked, this, [this, questionWidget]() {
        for(int i=0; i<ui->questionListWidget->count(); ++i) {
             QListWidgetItem* item = ui->questionListWidget->item(i);
             if (ui->questionListWidget->itemWidget(item) == questionWidget) {
                 saveQuestion(i);
                 return;
             }
        }
    });

    mainLayout->addWidget(saveQuestionBtn);
    
    questionWidget->setLayout(mainLayout);
    questionWidget->setStyleSheet("background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px;");
    
    item->setSizeHint(QSize(0, 350));
    ui->questionListWidget->addItem(item);
    ui->questionListWidget->setItemWidget(item, questionWidget);
}

void CreateExam::onAddAnswerButtonClicked() {}

void CreateExam::onDeleteQuestionClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    // Tìm widget cha (có thể là questionWidget hoặc headerLayout)
    QWidget *questionWidget = btn->parentWidget();
    while (questionWidget && questionWidget->parentWidget() != ui->questionListWidget->viewport()) {
        questionWidget = questionWidget->parentWidget();
    }
    
    if (!questionWidget) return;
    
    for (int i = 0; i < ui->questionListWidget->count(); i++) {
        QListWidgetItem *item = ui->questionListWidget->item(i);
        if (ui->questionListWidget->itemWidget(item) == questionWidget) {
            // Lấy question_id nếu có (câu hỏi đã lưu vào DB)
            int questionId = item->data(Qt::UserRole).toInt();
            
            if (questionId > 0) {
                // Xóa khỏi DB
                QMessageBox::StandardButton reply = QMessageBox::question(this, "Xác nhận", 
                    "Bạn có chắc muốn xóa câu hỏi này khỏi đề thi?",
                    QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::No) return;
                
                deleteQuestionFromExam(questionId);
            }
            
            // Xóa khỏi UI
            delete ui->questionListWidget->takeItem(i);
            break;
        }
    }
}

void CreateExam::deleteQuestionFromExam(int questionId)
{
    QJsonObject json;
    json["question_id"] = questionId;
    QJsonDocument doc(json);
    QString req = QString("CONTROL DELETE_EXAM_QUESTION\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, 8081);
    if (socket->waitForConnected(3000)) {
        socket->write(req.toUtf8());
        socket->flush();
        socket->waitForReadyRead(3000);
    }
    socket->close();
    socket->deleteLater();
}

void CreateExam::onDeleteAnswerClicked() {}

void CreateExam::saveQuestion(int questionIndex)
{
    if (questionIndex < 0 || questionIndex >= ui->questionListWidget->count()) return;
    
    QListWidgetItem *item = ui->questionListWidget->item(questionIndex);
    QWidget *questionWidget = ui->questionListWidget->itemWidget(item);
    
    QPlainTextEdit *contentEdit = questionWidget->findChild<QPlainTextEdit*>("contentEdit");
    QLineEdit *optA = questionWidget->findChild<QLineEdit*>("optionA");
    QLineEdit *optB = questionWidget->findChild<QLineEdit*>("optionB");
    QLineEdit *optC = questionWidget->findChild<QLineEdit*>("optionC");
    QLineEdit *optD = questionWidget->findChild<QLineEdit*>("optionD");
    QRadioButton *radA = questionWidget->findChild<QRadioButton*>("checkA");
    QRadioButton *radB = questionWidget->findChild<QRadioButton*>("checkB");
    QRadioButton *radC = questionWidget->findChild<QRadioButton*>("checkC");
    QRadioButton *radD = questionWidget->findChild<QRadioButton*>("checkD");
    
    if (!contentEdit || !optA || !optB || !optC || !optD) return;
    
    QString content = contentEdit->toPlainText().trimmed();
    QString strA = optA->text().trimmed();
    QString strB = optB->text().trimmed();
    QString strC = optC->text().trimmed();
    QString strD = optD->text().trimmed();

    if (content.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Vui lòng nhập nội dung câu hỏi!"); return; }
    if (strA.isEmpty() || strB.isEmpty() || strC.isEmpty() || strD.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Vui lòng nhập đủ 4 đáp án!"); return; }

    QString correctOption = "";
    if (radA && radA->isChecked()) correctOption = "A";
    else if (radB && radB->isChecked()) correctOption = "B";
    else if (radC && radC->isChecked()) correctOption = "C";
    else if (radD && radD->isChecked()) correctOption = "D";

    if (correctOption.isEmpty()) { QMessageBox::warning(this, "Lỗi", "Vui lòng chọn đáp án đúng!"); return; }
    
    QJsonObject json;
    json["exam_id"] = currentExamId;
    json["content"] = content;
    json["option_a"] = strA;
    json["option_b"] = strB;
    json["option_c"] = strC;
    json["option_d"] = strD;
    json["correct_option"] = correctOption;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL ADD_QUESTION_TO_EXAM\n%1").arg(QString(jsonData));
    
    QTcpSocket *saveSocket = new QTcpSocket(this);
    saveSocket->connectToHost(IPADDRESS, 8081);
    
    if (saveSocket->waitForConnected(3000)) {
        saveSocket->write(dataString.toUtf8());
        saveSocket->flush();
        
        if (saveSocket->waitForReadyRead(3000)) {
            QByteArray response = saveSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("ADD_QUESTION_TO_EXAM_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Đã lưu câu hỏi vào đề thi!");
                contentEdit->setReadOnly(true);
                optA->setReadOnly(true); optB->setReadOnly(true); optC->setReadOnly(true); optD->setReadOnly(true);
                radA->setEnabled(false); radB->setEnabled(false); radC->setEnabled(false); radD->setEnabled(false);
                QPushButton *saveBtn = questionWidget->findChild<QPushButton*>("saveQuestionBtn");
                if (saveBtn) {
                    saveBtn->setEnabled(false);
                    saveBtn->setText("✓ Đã lưu");
                    saveBtn->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 20px;");
                }
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể lưu câu hỏi! Server trả về: " + responseString);
            }
        }
    }
    saveSocket->close();
    delete saveSocket;
}

void CreateExam::loadExamQuestions() {
    if (currentExamId <= 0) {
        qDebug() << "loadExamQuestions: Invalid exam_id" << currentExamId;
        return;
    }
    
    ui->questionListWidget->clear();
    
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, 8081);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "loadExamQuestions: Cannot connect to server";
        socket->deleteLater();
        return;
    }
    
    QJsonObject json;
    json["exam_id"] = currentExamId;
    QJsonDocument doc(json);
    QString req = QString("CONTROL GET_EXAM_QUESTIONS\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    socket->write(req.toUtf8());
    socket->flush();
    
    if (socket->waitForReadyRead(5000)) {
        QString res(socket->readAll());
        qDebug() << "loadExamQuestions response:" << res.left(500);
        
        int idx = res.indexOf('{');
        if (idx != -1) {
            QJsonDocument d = QJsonDocument::fromJson(res.mid(idx).toUtf8());
            QJsonObject rootObj = d.object();
            
            // Backend trả về {"data": [...]} không phải {"questions": [...]}
            QJsonArray arr = rootObj["data"].toArray();
            
            qDebug() << "Found" << arr.size() << "questions";
            
            int questionNum = 1;
            for (const auto &val : arr) {
                QJsonObject obj = val.toObject();
                int questionId = obj["id"].toInt();
                QString content = obj["content"].toString();
                QString optA = obj["option_a"].toString();
                QString optB = obj["option_b"].toString();
                QString optC = obj["option_c"].toString();
                QString optD = obj["option_d"].toString();
                QString correct = obj["correct_option"].toString();
                
                // Tạo widget cho câu hỏi đã lưu (có nút xóa)
                QListWidgetItem *item = new QListWidgetItem();
                QWidget *questionWidget = new QWidget();
                QVBoxLayout *layout = new QVBoxLayout(questionWidget);
                layout->setContentsMargins(10, 5, 10, 5);
                
                // Header với số câu và nút xóa
                QHBoxLayout *headerLayout = new QHBoxLayout();
                QLabel *numLabel = new QLabel(QString("<b>Câu %1:</b> %2").arg(questionNum++).arg(content));
                numLabel->setWordWrap(true);
                headerLayout->addWidget(numLabel, 1);
                
                QPushButton *deleteBtn = new QPushButton("🗑 Xóa");
                deleteBtn->setStyleSheet("background-color: #dc3545; color: white; padding: 3px 8px;");
                deleteBtn->setCursor(Qt::PointingHandCursor);
                deleteBtn->setFixedWidth(60);
                connect(deleteBtn, &QPushButton::clicked, this, &CreateExam::onDeleteQuestionClicked);
                headerLayout->addWidget(deleteBtn);
                
                layout->addLayout(headerLayout);
                
                // Hiển thị đáp án
                QLabel *optionsLabel = new QLabel(QString("A. %1 | B. %2 | C. %3 | D. %4 <b>(Đúng: %5)</b>")
                    .arg(optA).arg(optB).arg(optC).arg(optD).arg(correct));
                optionsLabel->setStyleSheet("color: #555; margin-left: 10px;");
                optionsLabel->setWordWrap(true);
                layout->addWidget(optionsLabel);
                
                questionWidget->setLayout(layout);
                questionWidget->setStyleSheet("background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 5px;");
                
                item->setSizeHint(QSize(0, 80));
                item->setData(Qt::UserRole, questionId); // Lưu question_id để xóa
                ui->questionListWidget->addItem(item);
                ui->questionListWidget->setItemWidget(item, questionWidget);
            }
        } else {
            qDebug() << "Invalid response format:" << res;
        }
    } else {
        qDebug() << "Timeout waiting for GET_EXAM_QUESTIONS response";
    }
    
    socket->close();
    socket->deleteLater();
}

void CreateExam::importQuestion(int questionId) {
    QJsonObject json;
    json["exam_id"] = currentExamId;
    json["question_id"] = questionId;
    QJsonDocument doc(json);
    QString req = QString("CONTROL IMPORT_QUESTION_FROM_BANK\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(IPADDRESS, 8081);
    if(socket->waitForConnected(3000)) {
        socket->write(req.toUtf8());
        socket->flush();
        if(socket->waitForReadyRead(5000)) { // Tăng timeout
            QString res(socket->readAll());
            qDebug() << "Import response:" << res; // Debug log
            if(res.contains("IMPORT_QUESTION_SUCCESS") || res.contains("SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Đã nhập câu hỏi vào đề thi");
                // Delay nhỏ để đảm bảo server đã lưu xong
                QTimer::singleShot(200, this, [this]() {
                    loadExamQuestions(); // Reload để hiển thị đầy đủ câu hỏi
                });
            } else {
                 QMessageBox::warning(this, "Lỗi", "Không thể nhập câu hỏi: " + res);
            }
        } else {
            QMessageBox::warning(this, "Lỗi", "Timeout: Server không phản hồi");
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể kết nối server");
    }
    socket->close();
    socket->deleteLater();
}
