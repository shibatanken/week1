#include "classdetail.h"
#include "ui_classdetail.h"
#include "userdata.h"
#include "config.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QDebug>

ClassDetail::ClassDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassDetail),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1)
{
    ui->setupUi(this);
}

ClassDetail::~ClassDetail()
{
    delete ui;
}

void ClassDetail::setClassId(int classId)
{
    currentClassId = classId;
    loadClassDetail();
    loadStudentsInClass();
}

void ClassDetail::loadClassDetail()
{
    QJsonObject json;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL GET_CLASS_DETAIL\n%1").arg(QString(jsonData));
    
    QTcpSocket *detailSocket = new QTcpSocket(this);
    detailSocket->connectToHost(IPADDRESS, 8081);
    
    if (detailSocket->waitForConnected(3000)) {
        detailSocket->write(dataString.toUtf8());
        detailSocket->flush();
        
        if (detailSocket->waitForReadyRead(3000)) {
            QByteArray response = detailSocket->readAll();
            QString responseString(response);
            
            if (responseString.startsWith("DATA JSON GET_CLASS_DETAIL")) {
                int jsonStartIndex = responseString.indexOf('{');
                if (jsonStartIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex);
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject classObj = jsonDoc.object();
                    
                    // Update UI với thông tin lớp
                    ui->classNameLabel->setText(classObj["class_name"].toString());
                    ui->descriptionLabel->setText(classObj["description"].toString());
                    ui->teacherLabel->setText("Giáo viên: " + classObj["teacher_name"].toString());
                }
            }
        }
    }
    
    detailSocket->close();
    delete detailSocket;
}

void ClassDetail::loadStudentsInClass()
{
    QJsonObject json;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL GET_STUDENTS_IN_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *studentSocket = new QTcpSocket(this);
    studentSocket->connectToHost(IPADDRESS, 8081);
    
    if (studentSocket->waitForConnected(3000)) {
        studentSocket->write(dataString.toUtf8());
        studentSocket->flush();
        
        if (studentSocket->waitForReadyRead(3000)) {
            QByteArray response = studentSocket->readAll();
            QString responseString(response);
            
            if (responseString.startsWith("DATA JSON STUDENTS_IN_CLASS")) {
                int jsonStartIndex = responseString.indexOf('{');
                int jsonEndIndex = responseString.lastIndexOf('}');
                
                if (jsonStartIndex != -1 && jsonEndIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject jsonObj = jsonDoc.object();
                    QJsonArray students = jsonObj["data"].toArray();
                    
                    populateStudentList(students, true);
                }
            }
        }
    }
    
    studentSocket->close();
    delete studentSocket;
}

void ClassDetail::loadStudentsNotInClass()
{
    QJsonObject json;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL GET_STUDENTS_NOT_IN_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *studentSocket = new QTcpSocket(this);
    studentSocket->connectToHost(IPADDRESS, 8081);
    
    if (studentSocket->waitForConnected(3000)) {
        studentSocket->write(dataString.toUtf8());
        studentSocket->flush();
        
        if (studentSocket->waitForReadyRead(3000)) {
            QByteArray response = studentSocket->readAll();
            QString responseString(response);
            
            if (responseString.startsWith("DATA JSON STUDENTS_NOT_IN_CLASS")) {
                int jsonStartIndex = responseString.indexOf('{');
                int jsonEndIndex = responseString.lastIndexOf('}');
                
                if (jsonStartIndex != -1 && jsonEndIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject jsonObj = jsonDoc.object();
                    QJsonArray students = jsonObj["data"].toArray();
                    
                    // Populate combo box để thêm sinh viên
                    ui->studentComboBox->clear();
                    foreach (const QJsonValue &value, students) {
                        QJsonObject student = value.toObject();
                        QString displayText = QString("%1 (%2)")
                            .arg(student["name"].toString())
                            .arg(student["email"].toString());
                        ui->studentComboBox->addItem(displayText, student["id"].toInt());
                    }
                }
            }
        }
    }
    
    studentSocket->close();
    delete studentSocket;
}

void ClassDetail::populateStudentList(const QJsonArray &students, bool inClass)
{
    ui->studentListWidget->clear();
    
    QString userRole = UserData::instance().getRole();
    
    foreach (const QJsonValue &value, students) {
        QJsonObject student = value.toObject();
        
        QListWidgetItem *item = new QListWidgetItem(ui->studentListWidget);
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        
        // Student info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *nameLabel = new QLabel(student["name"].toString());
        QFont nameFont;
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        
        QLabel *emailLabel = new QLabel(student["email"].toString());
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(emailLabel);
        
        if (inClass && student.contains("joined_at")) {
            QLabel *joinedLabel = new QLabel("Tham gia: " + student["joined_at"].toString());
            joinedLabel->setStyleSheet("color: gray; font-size: 10px;");
            infoLayout->addWidget(joinedLabel);
        }
        
        layout->addLayout(infoLayout);
        layout->addStretch();
        
        // Remove button (chỉ teacher)
        if (userRole == "teacher" && inClass) {
            QPushButton *removeButton = new QPushButton("Xóa");
            removeButton->setCursor(Qt::PointingHandCursor);
            removeButton->setStyleSheet("background-color: #dc3545; color: white; padding: 5px 15px;");
            
            int userId = student["id"].toInt();
            connect(removeButton, &QPushButton::clicked, [this, userId]() {
                handleRemoveStudent(userId);
            });
            
            layout->addWidget(removeButton);
        }
        
        itemWidget->setLayout(layout);
        item->setSizeHint(itemWidget->sizeHint());
        ui->studentListWidget->addItem(item);
        ui->studentListWidget->setItemWidget(item, itemWidget);
    }
}

void ClassDetail::on_addStudentButton_clicked()
{
    // Chỉ teacher mới được thêm sinh viên
    if (UserData::instance().getRole() != "teacher") {
        QMessageBox::warning(this, "Lỗi", "Chỉ giáo viên mới có thể thêm sinh viên!");
        return;
    }
    
    // Load danh sách sinh viên chưa có trong lớp
    loadStudentsNotInClass();
    
    // Show dialog để chọn sinh viên
    if (ui->studentComboBox->count() == 0) {
        QMessageBox::information(this, "Thông báo", "Không còn sinh viên nào để thêm!");
        return;
    }
    
    int selectedUserId = ui->studentComboBox->currentData().toInt();
    
    // Gửi request thêm sinh viên
    QJsonObject json;
    json["user_id"] = selectedUserId;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL ADD_STUDENT_TO_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *addSocket = new QTcpSocket(this);
    addSocket->connectToHost(IPADDRESS, 8081);
    
    if (addSocket->waitForConnected(3000)) {
        addSocket->write(dataString.toUtf8());
        addSocket->flush();
        
        if (addSocket->waitForReadyRead(3000)) {
            QByteArray response = addSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("ADD_STUDENT_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Thêm sinh viên thành công!");
                loadStudentsInClass(); // Refresh
            } else {
                QMessageBox::warning(this, "Lỗi", "Thêm sinh viên thất bại!");
            }
        }
    }
    
    addSocket->close();
    delete addSocket;
}

void ClassDetail::handleRemoveStudent(int userId)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận", "Bạn có chắc muốn xóa sinh viên này khỏi lớp?",
                                  QMessageBox::Yes|QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Gửi request xóa sinh viên
    QJsonObject json;
    json["user_id"] = userId;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL REMOVE_STUDENT_FROM_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *removeSocket = new QTcpSocket(this);
    removeSocket->connectToHost(IPADDRESS, 8081);
    
    if (removeSocket->waitForConnected(3000)) {
        removeSocket->write(dataString.toUtf8());
        removeSocket->flush();
        
        if (removeSocket->waitForReadyRead(3000)) {
            QByteArray response = removeSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("REMOVE_STUDENT_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Xóa sinh viên thành công!");
                loadStudentsInClass(); // Refresh
            } else {
                QMessageBox::warning(this, "Lỗi", "Xóa sinh viên thất bại!");
            }
        }
    }
    
    removeSocket->close();
    delete removeSocket;
}

void ClassDetail::on_backButton_clicked()
{
    emit showClassList();
}

void ClassDetail::on_refreshButton_clicked()
{
    loadClassDetail();
    loadStudentsInClass();
}

