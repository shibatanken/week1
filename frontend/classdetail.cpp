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
        ui->practiceButton->show(); // Sinh viên có thể luyện tập
    } else {
        ui->createExamButton->show();
        ui->membersButton->show();
        questionBankBtn->show();
        ui->practiceButton->hide(); // Giáo viên không cần luyện tập
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
    ui->examListWidget->addItem("⏳ Đang tải danh sách bài kiểm tra...");
    
    QString userRole = UserData::instance().getRole();
    bool isStudent = (userRole == "student");
    
    QJsonObject json; json["class_id"] = currentClassId;
    QJsonDocument doc(json);
    QString dataString = QString("CONTROL GET_EXAMS_IN_CLASS\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
    
    QTcpSocket *examSocket = new QTcpSocket(this);
    
    connect(examSocket, &QTcpSocket::connected, this, [this, examSocket, isStudent]() {
        QJsonObject json; json["class_id"] = currentClassId;
        QJsonDocument doc(json);
        QString dataString = QString("CONTROL GET_EXAMS_IN_CLASS\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
        examSocket->write(dataString.toUtf8());
        examSocket->flush();
    });
    
    connect(examSocket, &QTcpSocket::readyRead, this, [this, examSocket, isStudent]() {
        QString responseString(examSocket->readAll());
        ui->examListWidget->clear();
        
        qDebug() << "Response from server:" << responseString.left(200);
        
        if (responseString.contains("EXAMS_IN_CLASS")) {
            int idx = responseString.indexOf('{');
            if (idx != -1) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseString.mid(idx).toUtf8());
                QJsonObject rootObj = jsonDoc.object();
                QJsonArray examsArray = rootObj["data"].toArray();
                qDebug() << "Found" << examsArray.size() << "exams";
                
                int addedCount = 0;
                for (const QJsonValue &value : examsArray) {
                    QJsonObject examObj = value.toObject();
                    QString status = examObj["status"].toString();
                    
                    // Sinh viên chỉ thấy đề đã published hoặc ongoing
                    if (isStudent && status == "draft") {
                        continue;
                    }
                    
                    int examId = examObj["id"].toInt();
                    QString examTitle = examObj["exam_name"].toString();
                    int questionCount = examObj["question_count"].toInt();
                    int timeLimit = examObj["time_limit"].toInt();
                    
                    QString displayText;
                    if (isStudent) {
                        // Sinh viên: hiển thị đơn giản hơn
                        if (status == "published") {
                            displayText = QString("📝 %1 (%2 câu, %3 phút)").arg(examTitle).arg(questionCount).arg(timeLimit);
                        } else if (status == "ongoing") {
                            displayText = QString("🔴 %1 - ĐANG DIỄN RA (%2 câu)").arg(examTitle).arg(questionCount);
                        } else {
                            displayText = QString("✅ %1 - Đã kết thúc").arg(examTitle);
                        }
                    } else {
                        // Giáo viên: hiển thị đầy đủ
                        displayText = QString("%1 - %2 (%3 câu)").arg(examTitle).arg(status).arg(questionCount);
                    }
                    
                    QListWidgetItem *item = new QListWidgetItem(displayText);
                    item->setData(Qt::UserRole, examId);
                    item->setData(Qt::UserRole + 1, examTitle);
                    item->setData(Qt::UserRole + 2, timeLimit);
                    item->setData(Qt::UserRole + 3, status);
                    
                    if (status == "closed" || status == "finished") {
                        item->setForeground(Qt::gray);
                    } else if (status == "ongoing") {
                        item->setForeground(Qt::red);
                        item->setFont(QFont("Arial", 12, QFont::Bold));
                    } else {
                        item->setForeground(Qt::black);
                        item->setFont(QFont("Arial", 11));
                    }
                    ui->examListWidget->addItem(item);
                    addedCount++;
                }
                
                if (addedCount == 0) {
                    if (isStudent) {
                        ui->examListWidget->addItem("📭 Chưa có bài kiểm tra nào được công bố.");
                    } else {
                        ui->examListWidget->addItem("📭 Chưa có bài kiểm tra nào. Bấm 'Tạo bài kiểm tra' để bắt đầu.");
                    }
                }
            }
        } else {
            ui->examListWidget->addItem("❌ Lỗi tải danh sách bài kiểm tra.");
        }
        
        examSocket->close();
        examSocket->deleteLater();
    });
    
    connect(examSocket, &QTcpSocket::errorOccurred, this, [this, examSocket](QAbstractSocket::SocketError) {
        ui->examListWidget->clear();
        ui->examListWidget->addItem("❌ Lỗi kết nối server: " + examSocket->errorString());
        examSocket->deleteLater();
    });
    
    examSocket->connectToHost(IPADDRESS, 8081);
}

void ClassDetail::on_backButton_clicked() { emit backToClassList(); }
void ClassDetail::on_createExamButton_clicked() { emit openCreateExam(currentClassId); }
void ClassDetail::on_membersButton_clicked() { emit openClassMembers(currentClassId, currentClassName); }
void ClassDetail::on_practiceButton_clicked() { emit openPracticeMode(currentClassId, currentClassName); }

void ClassDetail::onExamItemClicked(QListWidgetItem *item)
{
    int examId = item->data(Qt::UserRole).toInt();
    if (examId <= 0) return; // Ignore placeholder items

    QString userRole = UserData::instance().getRole();

    if (userRole == "student") {
        // Sinh viên → mở làm bài thi hoặc xem kết quả
        QString examName = item->data(Qt::UserRole + 1).toString();
        int timeLimit = item->data(Qt::UserRole + 2).toInt();
        QString status = item->data(Qt::UserRole + 3).toString();

        if (status == "finished" || status == "closed") {
            // Bài thi đã kết thúc → lấy submission_id và xem kết quả
            QTcpSocket socket;
            socket.connectToHost(IPADDRESS, PORT);
            if (!socket.waitForConnected(3000)) {
                QMessageBox::critical(this, "Lỗi", "Không thể kết nối server!");
                return;
            }

            QJsonObject json;
            json["exam_id"] = examId;
            json["user_id"] = UserData::instance().getUserId();

            QString request = QString("CONTROL GET_SUBMISSION_STATUS\n%1")
                .arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
            socket.write(request.toUtf8());
            socket.flush();

            if (!socket.waitForReadyRead(5000)) {
                QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
                socket.close();
                return;
            }

            QByteArray response = socket.readAll();
            QString responseStr(response);
            socket.close();

            int jsonStart = responseStr.indexOf('{');
            if (jsonStart == -1) {
                QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
                return;
            }

            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            QJsonObject obj = doc.object();
            int submissionId = obj["submission_id"].toInt();

            if (submissionId > 0) {
                emit viewExamResult(submissionId);
            } else {
                QMessageBox::information(this, "Thông báo", "Bạn chưa tham gia bài thi này.");
            }
            return;
        }

        emit startExamForStudent(examId, examName, timeLimit);
    } else {
        // Giáo viên → mở chỉnh sửa đề
        emit openExamDetail(examId);
    }
}

void ClassDetail::on_refreshButton_clicked() { loadExamsInClass(); }

