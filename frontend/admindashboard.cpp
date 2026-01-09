#include "admindashboard.h"
#include "ui_admindashboard.h"
#include "config.h"
#include "userdata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QInputDialog>
#include <QTableWidget>
#include <QHeaderView>

AdminDashboard::AdminDashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminDashboard),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    
    connect(ui->btnLogout, &QPushButton::clicked, this, &AdminDashboard::logout);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &AdminDashboard::loadData);
    connect(ui->btnAddUser, &QPushButton::clicked, this, &AdminDashboard::onAddUser);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &AdminDashboard::onTabChanged);
}

AdminDashboard::~AdminDashboard()
{
    delete ui;
}

void AdminDashboard::loadData()
{
    if (ui->tabWidget->currentIndex() == 0) {
        loadPendingUsers();
    } else {
        loadAllUsers();
    }
}

void AdminDashboard::onTabChanged(int index)
{
    if (index == 0) {
        loadPendingUsers();
    } else {
        loadAllUsers();
    }
}

void AdminDashboard::loadPendingUsers()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QString request = "CONTROL ADMIN_GET_PENDING_USERS\n{}";
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

void AdminDashboard::loadAllUsers()
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QString request = "CONTROL ADMIN_GET_ALL_USERS\n{}";
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

void AdminDashboard::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QString responseStr(response);
    qDebug() << "Admin response:" << responseStr;
    
    tcpSocket->disconnect(SIGNAL(readyRead()));
    tcpSocket->close();
    
    if (responseStr.contains("PENDING_USERS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            pendingUsers = doc.object()["data"].toArray();
            displayPendingUsers();
        }
    }
    else if (responseStr.contains("ALL_USERS")) {
        int jsonStart = responseStr.indexOf('{');
        if (jsonStart != -1) {
            QJsonDocument doc = QJsonDocument::fromJson(responseStr.mid(jsonStart).toUtf8());
            allUsers = doc.object()["data"].toArray();
            displayAllUsers();
        }
    }
    else if (responseStr.contains("SUCCESS")) {
        QMessageBox::information(this, "Thành công", "Thao tác thành công!");
        loadData();
    }
}

void AdminDashboard::displayPendingUsers()
{
    ui->tablePending->setRowCount(0);
    ui->tablePending->setColumnCount(5);
    ui->tablePending->setHorizontalHeaderLabels({"ID", "Email", "Tên", "Vai trò", "Hành động"});
    ui->tablePending->horizontalHeader()->setStretchLastSection(true);
    
    for (int i = 0; i < pendingUsers.size(); i++) {
        QJsonObject user = pendingUsers[i].toObject();
        int row = ui->tablePending->rowCount();
        ui->tablePending->insertRow(row);
        
        ui->tablePending->setItem(row, 0, new QTableWidgetItem(QString::number(user["id"].toInt())));
        ui->tablePending->setItem(row, 1, new QTableWidgetItem(user["email"].toString()));
        ui->tablePending->setItem(row, 2, new QTableWidgetItem(user["name"].toString()));
        ui->tablePending->setItem(row, 3, new QTableWidgetItem(user["role"].toString()));
        
        // Action buttons
        QWidget *actionsWidget = new QWidget();
        QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(5, 2, 5, 2);
        
        int userId = user["id"].toInt();
        
        QPushButton *approveBtn = new QPushButton("✓");
        approveBtn->setStyleSheet("background-color: #4CAF50; color: white;");
        approveBtn->setToolTip("Duyệt");
        connect(approveBtn, &QPushButton::clicked, [this, userId]() {
            onApproveUser(userId);
        });
        actionsLayout->addWidget(approveBtn);
        
        QPushButton *rejectBtn = new QPushButton("✗");
        rejectBtn->setStyleSheet("background-color: #f44336; color: white;");
        rejectBtn->setToolTip("Từ chối");
        connect(rejectBtn, &QPushButton::clicked, [this, userId]() {
            onRejectUser(userId);
        });
        actionsLayout->addWidget(rejectBtn);
        
        ui->tablePending->setCellWidget(row, 4, actionsWidget);
    }
    
    ui->lblPendingCount->setText(QString("Đang chờ duyệt: %1").arg(pendingUsers.size()));
}

void AdminDashboard::displayAllUsers()
{
    ui->tableAllUsers->setRowCount(0);
    ui->tableAllUsers->setColumnCount(6);
    ui->tableAllUsers->setHorizontalHeaderLabels({"ID", "Email", "Tên", "Vai trò", "Trạng thái", "Hành động"});
    ui->tableAllUsers->horizontalHeader()->setStretchLastSection(true);
    
    for (int i = 0; i < allUsers.size(); i++) {
        QJsonObject user = allUsers[i].toObject();
        int row = ui->tableAllUsers->rowCount();
        ui->tableAllUsers->insertRow(row);
        
        ui->tableAllUsers->setItem(row, 0, new QTableWidgetItem(QString::number(user["id"].toInt())));
        ui->tableAllUsers->setItem(row, 1, new QTableWidgetItem(user["email"].toString()));
        ui->tableAllUsers->setItem(row, 2, new QTableWidgetItem(user["name"].toString()));
        ui->tableAllUsers->setItem(row, 3, new QTableWidgetItem(user["role"].toString()));
        ui->tableAllUsers->setItem(row, 4, new QTableWidgetItem(user["status"].toString()));
        
        // Delete button
        int userId = user["id"].toInt();
        QPushButton *deleteBtn = new QPushButton("🗑️ Xóa");
        deleteBtn->setStyleSheet("background-color: #f44336; color: white;");
        connect(deleteBtn, &QPushButton::clicked, [this, userId]() {
            onDeleteUser(userId);
        });
        ui->tableAllUsers->setCellWidget(row, 5, deleteBtn);
    }
    
    ui->lblTotalUsers->setText(QString("Tổng số: %1 người dùng").arg(allUsers.size()));
}

void AdminDashboard::onApproveUser(int userId)
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["user_id"] = userId;
        
        QString request = QString("CONTROL ADMIN_APPROVE_USER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

void AdminDashboard::onRejectUser(int userId)
{
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["user_id"] = userId;
        
        QString request = QString("CONTROL ADMIN_REJECT_USER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

void AdminDashboard::onDeleteUser(int userId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Xác nhận", 
        "Bạn có chắc muốn xóa người dùng này?", QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) return;
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["user_id"] = userId;
        
        QString request = QString("CONTROL ADMIN_DELETE_USER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

void AdminDashboard::onAddUser()
{
    bool ok;
    QString email = QInputDialog::getText(this, "Thêm người dùng", "Email:", QLineEdit::Normal, "", &ok);
    if (!ok || email.isEmpty()) return;
    
    QString name = QInputDialog::getText(this, "Thêm người dùng", "Tên:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    QString password = QInputDialog::getText(this, "Thêm người dùng", "Mật khẩu:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;
    
    QStringList roles = {"student", "teacher", "admin"};
    QString role = QInputDialog::getItem(this, "Thêm người dùng", "Vai trò:", roles, 0, false, &ok);
    if (!ok) return;
    
    tcpSocket->connectToHost(IPADDRESS, PORT);
    if (tcpSocket->waitForConnected(3000)) {
        QJsonObject json;
        json["email"] = email;
        json["name"] = name;
        json["password"] = password;
        json["role"] = role;
        
        QString request = QString("CONTROL ADMIN_ADD_USER\n%1").arg(QString(QJsonDocument(json).toJson(QJsonDocument::Compact)));
        tcpSocket->write(request.toUtf8());
        tcpSocket->flush();
        
        connect(tcpSocket, &QTcpSocket::readyRead, this, &AdminDashboard::onReadyRead);
    }
}

