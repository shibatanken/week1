#include "classlist.h"
#include "ui_classlist.h"
#include "userdata.h"
#include "config.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QDebug>

ClassList::ClassList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassList),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ClassList::onReadyRead);
    
    // Load class list khi widget được hiển thị
    loadClassList();
}

ClassList::~ClassList()
{
    delete ui;
}

void ClassList::loadClassList()
{
    QString dataString = QString("CONTROL GET_CLASS_LIST\n{}");
    
    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected(3000)) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
        qDebug() << "Sent GET_CLASS_LIST request";
    } else {
        qDebug() << "Failed to connect to server";
        QMessageBox::warning(this, "Lỗi", "Không thể kết nối đến server");
    }
}

void ClassList::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);
    
    qDebug() << "Received response:" << responseString;
    
    if (responseString.startsWith("DATA JSON GET_CLASS_LIST")) {
        int jsonStartIndex = responseString.indexOf('{');
        int jsonEndIndex = responseString.lastIndexOf('}');
        
        if (jsonStartIndex != -1 && jsonEndIndex != -1) {
            QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
            
            if (parseError.error == QJsonParseError::NoError) {
                QJsonObject jsonObj = jsonDoc.object();
                QJsonArray classList = jsonObj["data"].toArray();
                populateClassList(classList);
            } else {
                qDebug() << "JSON Parse Error:" << parseError.errorString();
            }
        }
    }
}

void ClassList::populateClassList(const QJsonArray &classes)
{
    ui->classListWidget->clear();
    
    QString userRole = UserData::instance().getRole();
    int currentUserId = UserData::instance().getUserId();
    
    foreach (const QJsonValue &value, classes) {
        QJsonObject classObj = value.toObject();
        
        // Nếu là student, chỉ hiển thị lớp đã join
        // Nếu là teacher, hiển thị lớp do mình tạo
        if (userRole == "student") {
            // TODO: Check if student is in this class
            createClassItem(classObj);
        } else if (userRole == "teacher") {
            if (classObj["teacher_id"].toInt() == currentUserId) {
                createClassItem(classObj);
            }
        }
    }
}

void ClassList::createClassItem(const QJsonObject &classObj)
{
    QListWidgetItem *item = new QListWidgetItem(ui->classListWidget);
    QWidget *itemWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(itemWidget);
    
    // Class name (bold)
    QLabel *nameLabel = new QLabel(classObj["class_name"].toString());
    QFont nameFont;
    nameFont.setPointSize(14);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    
    // Description
    QLabel *descLabel = new QLabel(classObj["description"].toString());
    descLabel->setWordWrap(true);
    
    // Teacher name
    QLabel *teacherLabel = new QLabel("Giáo viên: " + classObj["teacher_name"].toString());
    
    // Student count
    QLabel *countLabel = new QLabel("Số sinh viên: " + QString::number(classObj["student_count"].toInt()));
    
    // Created date
    QLabel *dateLabel = new QLabel("Ngày tạo: " + classObj["created_at"].toString());
    
    // View button
    QPushButton *viewButton = new QPushButton("Xem chi tiết");
    viewButton->setCursor(Qt::PointingHandCursor);
    viewButton->setStyleSheet("background-color: #007bff; color: white; padding: 5px 15px; border-radius: 3px;");
    
    int classId = classObj["id"].toInt();
    connect(viewButton, &QPushButton::clicked, [this, classId]() {
        emit showClassDetail(classId);
    });
    
    layout->addWidget(nameLabel);
    layout->addWidget(descLabel);
    layout->addWidget(teacherLabel);
    layout->addWidget(countLabel);
    layout->addWidget(dateLabel);
    layout->addWidget(viewButton);
    
    itemWidget->setLayout(layout);
    item->setSizeHint(itemWidget->sizeHint());
    ui->classListWidget->addItem(item);
    ui->classListWidget->setItemWidget(item, itemWidget);
}

void ClassList::on_createClassButton_clicked()
{
    // Chỉ teacher mới được tạo lớp
    if (UserData::instance().getRole() != "teacher") {
        QMessageBox::warning(this, "Lỗi", "Chỉ giáo viên mới có thể tạo lớp học!");
        return;
    }
    
    bool ok;
    QString className = QInputDialog::getText(this, "Tạo lớp học mới",
                                             "Tên lớp học:", QLineEdit::Normal,
                                             "", &ok);
    if (!ok || className.isEmpty()) {
        return;
    }
    
    QString description = QInputDialog::getText(this, "Tạo lớp học mới",
                                               "Mô tả:", QLineEdit::Normal,
                                               "", &ok);
    if (!ok) {
        return;
    }
    
    // Gửi request tạo lớp
    QJsonObject json;
    json["class_name"] = className;
    json["description"] = description;
    json["teacher_id"] = UserData::instance().getUserId();
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QString dataString = QString("CONTROL CREATE_CLASS\n%1").arg(QString(jsonData));
    
    QTcpSocket *createSocket = new QTcpSocket(this);
    createSocket->connectToHost(IPADDRESS, 8081);
    
    if (createSocket->waitForConnected(3000)) {
        createSocket->write(dataString.toUtf8());
        createSocket->flush();
        
        if (createSocket->waitForReadyRead(3000)) {
            QByteArray response = createSocket->readAll();
            QString responseString(response);
            
            if (responseString.contains("CREATE_CLASS_SUCCESS")) {
                QMessageBox::information(this, "Thành công", "Tạo lớp học thành công!");
                loadClassList(); // Refresh list
            } else {
                QMessageBox::warning(this, "Lỗi", "Tạo lớp học thất bại!");
            }
        }
    }
    
    createSocket->close();
    delete createSocket;
}

void ClassList::on_refreshButton_clicked()
{
    loadClassList();
}

void ClassList::handleClassItemClicked(int classId)
{
    emit showClassDetail(classId);
}

