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
#include <QDebug>

ClassDetail::ClassDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassDetail),
    tcpSocket(new QTcpSocket(this)),
    currentClassId(-1)
{
    ui->setupUi(this);
    // Không kiểm tra role ở đây nữa
}

ClassDetail::~ClassDetail()
{
    delete ui;
}

void ClassDetail::setClassId(int classId)
{
    currentClassId = classId;
    
    // Kiểm tra role mỗi khi setClassId được gọi
    QString userRole = UserData::instance().getRole();
    qDebug() << "ClassDetail - User role:" << userRole;
    
    if (userRole == "student") {
        ui->createExamButton->hide();
        ui->membersButton->hide();  // Học sinh không thấy nút thành viên
    } else {
        ui->createExamButton->show();
        ui->membersButton->show();
    }
    
    loadClassDetail();
    loadExamsInClass();
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
                    
                    currentClassName = classObj["class_name"].toString();
                    ui->classNameLabel->setText(currentClassName);
                    ui->descriptionLabel->setText(classObj["description"].toString());
                    ui->teacherLabel->setText("Giáo viên: " + classObj["teacher_name"].toString());
                }
            }
        }
    }
    
    detailSocket->close();
    delete detailSocket;
}

void ClassDetail::loadExamsInClass()
{
    QJsonObject json;
    json["class_id"] = currentClassId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL GET_EXAMS_IN_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *examSocket = new QTcpSocket(this);
    examSocket->connectToHost(IPADDRESS, 8081);
    
    if (examSocket->waitForConnected(3000)) {
        examSocket->write(dataString.toUtf8());
        examSocket->flush();
        
        if (examSocket->waitForReadyRead(3000)) {
            QByteArray response = examSocket->readAll();
            QString responseString(response);
            
            if (responseString.startsWith("DATA JSON EXAMS_IN_CLASS")) {
                int jsonStartIndex = responseString.indexOf('{');
                int jsonEndIndex = responseString.lastIndexOf('}');
                
                if (jsonStartIndex != -1 && jsonEndIndex != -1) {
                    QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
                    QJsonObject jsonObj = jsonDoc.object();
                    QJsonArray exams = jsonObj["data"].toArray();
                    
                    populateExamList(exams);
                }
            }
        }
    }
    
    examSocket->close();
    delete examSocket;
}

void ClassDetail::populateExamList(const QJsonArray &exams)
{
    ui->examListWidget->clear();
    
    QString userRole = UserData::instance().getRole();
    
    if (exams.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem(ui->examListWidget);
        QLabel *emptyLabel = new QLabel("Chưa có bài kiểm tra nào trong lớp này.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; padding: 20px;");
        emptyItem->setSizeHint(emptyLabel->sizeHint());
        ui->examListWidget->addItem(emptyItem);
        ui->examListWidget->setItemWidget(emptyItem, emptyLabel);
        return;
    }
    
    foreach (const QJsonValue &value, exams) {
        QJsonObject exam = value.toObject();
        int examId = exam["id"].toInt();
        
        QListWidgetItem *item = new QListWidgetItem(ui->examListWidget);
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(10, 10, 10, 10);
        
        // Exam info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        
        // Tên bài kiểm tra
        QLabel *nameLabel = new QLabel(exam["exam_name"].toString());
        QFont nameFont;
        nameFont.setBold(true);
        nameFont.setPointSize(12);
        nameLabel->setFont(nameFont);
        
        // Mô tả
        QLabel *descLabel = new QLabel(exam["description"].toString());
        descLabel->setStyleSheet("color: gray;");
        
        // Thông tin thêm
        QString statusText;
        QString status = exam["status"].toString();
        if (status == "draft") statusText = "Nháp";
        else if (status == "published") statusText = "Đã công bố";
        else if (status == "ongoing") statusText = "Đang diễn ra";
        else if (status == "finished") statusText = "Đã kết thúc";
        
        QLabel *infoLabel = new QLabel(QString("⏱ %1 phút | 📝 %2 câu hỏi | 📌 %3")
            .arg(exam["time_limit"].toInt())
            .arg(exam["question_count"].toInt())
            .arg(statusText));
        infoLabel->setStyleSheet("color: #666; font-size: 11px;");
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(descLabel);
        infoLayout->addWidget(infoLabel);
        
        layout->addLayout(infoLayout);
        layout->addStretch();
        
        // Buttons
        QVBoxLayout *buttonLayout = new QVBoxLayout();
        
        // Nút xem chi tiết
        QPushButton *viewButton = new QPushButton("Xem chi tiết");
        viewButton->setCursor(Qt::PointingHandCursor);
        viewButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 15px; border-radius: 4px;");
        connect(viewButton, &QPushButton::clicked, [this, examId]() {
            handleExamItemClicked(examId);
        });
        buttonLayout->addWidget(viewButton);
        
        // Nút xóa (chỉ teacher)
        if (userRole == "teacher") {
            QPushButton *deleteButton = new QPushButton("Xóa");
            deleteButton->setCursor(Qt::PointingHandCursor);
            deleteButton->setStyleSheet("background-color: #dc3545; color: white; padding: 8px 15px; border-radius: 4px;");
            connect(deleteButton, &QPushButton::clicked, [this, examId]() {
                handleDeleteExam(examId);
            });
            buttonLayout->addWidget(deleteButton);
        }
        
        layout->addLayout(buttonLayout);
        
        itemWidget->setLayout(layout);
        item->setSizeHint(QSize(0, 100));
        ui->examListWidget->addItem(item);
        ui->examListWidget->setItemWidget(item, itemWidget);
    }
}

void ClassDetail::on_backButton_clicked()
{
    emit showClassList();
}

void ClassDetail::on_membersButton_clicked()
{
    emit showClassMembers(currentClassId, currentClassName);
}

void ClassDetail::on_createExamButton_clicked()
{
    emit showCreateExam(currentClassId);
}

void ClassDetail::on_refreshButton_clicked()
{
    loadExamsInClass();
}

void ClassDetail::handleExamItemClicked(int examId)
{
    emit showExamDetail(examId);
}

void ClassDetail::handleDeleteExam(int examId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Xác nhận xóa", 
        "Bạn có chắc chắn muốn xóa bài kiểm tra này?\nTất cả câu hỏi trong bài kiểm tra cũng sẽ bị xóa.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) return;
    
    QJsonObject json;
    json["exam_id"] = examId;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL DELETE_EXAM\n%1").arg(QString(jsonData));
    
    QTcpSocket *deleteSocket = new QTcpSocket(this);
    deleteSocket->connectToHost(IPADDRESS, 8081);
    
    if (deleteSocket->waitForConnected(3000)) {
        deleteSocket->write(dataString.toUtf8());
        deleteSocket->flush();
        
        if (deleteSocket->waitForReadyRead(3000)) {
            QByteArray response = deleteSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("DELETE_EXAM_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Đã xóa bài kiểm tra!");
                loadExamsInClass();
            } else {
                QMessageBox::warning(this, "Lỗi", "Không thể xóa bài kiểm tra!");
            }
        }
    }
    
    deleteSocket->close();
    delete deleteSocket;
}