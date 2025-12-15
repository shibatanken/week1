#include "classmembers.h"
#include "ui_classmembers.h"
#include "userdata.h"
#include "config.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ClassMembers::ClassMembers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassMembers),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1)
{
    ui->setupUi(this);
    // Không kiểm tra role ở đây nữa
}

ClassMembers::~ClassMembers()
{
    delete ui;
}

void ClassMembers::setClassInfo(int classId, const QString &className)
{
    currentClassId = classId;
    currentClassName = className;
    ui->titleLabel->setText("Thành viên lớp: " + className);
    
    // Kiểm tra role mỗi khi setClassInfo được gọi
    QString userRole = UserData::instance().getRole();
    qDebug() << "ClassMembers - User role:" << userRole;
    
    if (userRole != "teacher") {
        ui->addStudentWidget->hide();
    } else {
        ui->addStudentWidget->show();
    }
    
    loadStudentsInClass();
    loadStudentsNotInClass();
}


void ClassMembers::loadStudentsInClass()
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
                    
                    populateStudentList(students);
                }
            }
        }
    }
    
    studentSocket->close();
    delete studentSocket;
}

void ClassMembers::loadStudentsNotInClass()
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

void ClassMembers::populateStudentList(const QJsonArray &students)
{
    ui->studentListWidget->clear();
    
    QString userRole = UserData::instance().getRole();
    
    if (students.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem(ui->studentListWidget);
        QLabel *emptyLabel = new QLabel("Chưa có sinh viên nào trong lớp.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; padding: 20px;");
        emptyItem->setSizeHint(emptyLabel->sizeHint());
        ui->studentListWidget->addItem(emptyItem);
        ui->studentListWidget->setItemWidget(emptyItem, emptyLabel);
        return;
    }
    
    foreach (const QJsonValue &value, students) {
        QJsonObject student = value.toObject();
        
        QListWidgetItem *item = new QListWidgetItem(ui->studentListWidget);
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(10, 8, 10, 8);
        
        // Student info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *nameLabel = new QLabel(student["name"].toString());
        QFont nameFont;
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        
        QLabel *emailLabel = new QLabel(student["email"].toString());
        emailLabel->setStyleSheet("color: gray;");
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(emailLabel);
        
        if (student.contains("joined_at")) {
            QLabel *joinedLabel = new QLabel("Tham gia: " + student["joined_at"].toString());
            joinedLabel->setStyleSheet("color: #999; font-size: 10px;");
            infoLayout->addWidget(joinedLabel);
        }
        
        layout->addLayout(infoLayout);
        layout->addStretch();
        
        // Remove button (chỉ teacher)
        if (userRole == "teacher") {
            QPushButton *removeButton = new QPushButton("Xóa");
            removeButton->setCursor(Qt::PointingHandCursor);
            removeButton->setStyleSheet("background-color: #dc3545; color: white; padding: 5px 15px; border-radius: 4px;");
            
            int userId = student["id"].toInt();
            connect(removeButton, &QPushButton::clicked, [this, userId]() {
                handleRemoveStudent(userId);
            });
            
            layout->addWidget(removeButton);
        }
        
        itemWidget->setLayout(layout);
        item->setSizeHint(QSize(0, 70));
        ui->studentListWidget->addItem(item);
        ui->studentListWidget->setItemWidget(item, itemWidget);
    }
}

void ClassMembers::on_backButton_clicked()
{
    emit backToClassDetail();
}

void ClassMembers::on_addStudentButton_clicked()
{
    if (ui->studentComboBox->count() == 0) {
        QMessageBox::information(this, "Thông báo", "Không còn sinh viên nào để thêm!");
        return;
    }
    
    int selectedUserId = ui->studentComboBox->currentData().toInt();
    
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
                QMessageBox::information(this, "Thành công", "Đã thêm sinh viên vào lớp!");
                loadStudentsInClass();
                loadStudentsNotInClass();
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể thêm sinh viên!");
            }
        }
    }
    
    addSocket->close();
    delete addSocket;
}

void ClassMembers::on_refreshButton_clicked()
{
    loadStudentsInClass();
    loadStudentsNotInClass();
}

void ClassMembers::handleRemoveStudent(int userId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Xác nhận xóa", 
        "Bạn có chắc chắn muốn xóa sinh viên này khỏi lớp?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) return;
    
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
                QMessageBox::information(this, "Thành công", "Đã xóa sinh viên khỏi lớp!");
                loadStudentsInClass();
                loadStudentsNotInClass();
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể xóa sinh viên!");
            }
        }
    }
    
    removeSocket->close();
    delete removeSocket;
}