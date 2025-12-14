#include "examroommanagement.h"
#include "ui_examroommanagement.h"
#include "createexamroom.h"
#include "examroomdialog.h"
#include "userdata.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QHeaderView>
#include "config.h"

ExamRoomManagement::ExamRoomManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamRoomManagement),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // Setup table
    ui->roomTableWidget->setColumnCount(8);
    QStringList headers = {"ID", "Tên phòng", "Mô tả", "Thời gian", "Câu dễ", "Câu TB", "Câu khó", "Hành động"};
    ui->roomTableWidget->setHorizontalHeaderLabels(headers);
    
    QHeaderView *header = ui->roomTableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    QFont headerFont = header->font();
    headerFont.setBold(true);
    header->setFont(headerFont);

    // Update user info
    ui->userNameLabel->setText("Xin chào, " + UserData::instance().getUserName());

    connect(ui->createRoomButton, &QPushButton::clicked, this, &ExamRoomManagement::onCreateExamRoomClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ExamRoomManagement::onLogoutClicked);

    // Load room list
    handleGetExamRoomList();
}

ExamRoomManagement::~ExamRoomManagement()
{
    delete ui;
}

void ExamRoomManagement::onCreateExamRoomClicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Tạo Phòng Thi");
    dialog->resize(500, 500);

    CreateExamRoom *createRoom = new CreateExamRoom(dialog);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(createRoom);
    dialog->setLayout(layout);

    connect(createRoom, &CreateExamRoom::roomCreated, this, &ExamRoomManagement::refreshRoomList);

    dialog->exec();
}

void ExamRoomManagement::onViewRoomClicked(int row)
{
    QString roomId = ui->roomTableWidget->item(row, 0)->text();
    QString roomName = ui->roomTableWidget->item(row, 1)->text();
    QString description = ui->roomTableWidget->item(row, 2)->text();
    QString timeLimit = ui->roomTableWidget->item(row, 3)->text();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Chi tiết phòng thi - " + roomName);
    dialog->resize(800, 600);

    ExamRoomDialog *roomDialog = new ExamRoomDialog(dialog);
    roomDialog->setRoomDetails(roomId.toInt(), roomName, description, timeLimit);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(roomDialog);
    dialog->setLayout(layout);

    connect(roomDialog, &ExamRoomDialog::roomUpdated, this, &ExamRoomManagement::refreshRoomList);

    dialog->exec();
}

void ExamRoomManagement::refreshRoomList()
{
    handleGetExamRoomList();
}

void ExamRoomManagement::handleGetExamRoomList()
{
    QString dataString = QString("CONTROL GET_ROOM_LIST\n{}");

    tcpSocket->connectToHost(IPADDRESS, 8081);
    if (tcpSocket->waitForConnected()) {
        tcpSocket->write(dataString.toUtf8());
        tcpSocket->flush();
    } else {
        qDebug() << "Failed to connect to server";
        return;
    }

    connect(tcpSocket, &QTcpSocket::readyRead, this, &ExamRoomManagement::handleGetExamRoomListResponse);
}

void ExamRoomManagement::handleGetExamRoomListResponse()
{
    QByteArray response = tcpSocket->readAll();
    QString responseString(response);

    qDebug() << "Get room list response:" << responseString;

    if (responseString.startsWith("DATA JSON GET_ROOM_LIST")) {
        int jsonStartIndex = responseString.indexOf('{');
        int jsonEndIndex = responseString.lastIndexOf('}');
        
        if (jsonStartIndex != -1 && jsonEndIndex != -1) {
            QString jsonString = responseString.mid(jsonStartIndex, jsonEndIndex - jsonStartIndex + 1);
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
            
            if (parseError.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = jsonDoc.object();
                QJsonArray roomList = jsonObject["data"].toArray();

                ui->roomTableWidget->setRowCount(0);

                for (int i = 0; i < roomList.size(); ++i) {
                    QJsonObject room = roomList[i].toObject();
                    
                    ui->roomTableWidget->insertRow(i);
                    ui->roomTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(room["id"].toInt())));
                    ui->roomTableWidget->setItem(i, 1, new QTableWidgetItem(room["subject"].toString()));
                    ui->roomTableWidget->setItem(i, 2, new QTableWidgetItem(room["description"].toString()));
                    ui->roomTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(room["time_limit"].toInt()) + " phút"));
                    ui->roomTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(room["number_of_easy_question"].toInt())));
                    ui->roomTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(room["number_of_medium_question"].toInt())));
                    ui->roomTableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(room["number_of_hard_question"].toInt())));

                    QPushButton *viewButton = new QPushButton("Xem");
                    viewButton->setCursor(Qt::PointingHandCursor);
                    ui->roomTableWidget->setCellWidget(i, 7, viewButton);

                    connect(viewButton, &QPushButton::clicked, [this, i]() {
                        onViewRoomClicked(i);
                    });
                }
            }
        }
    }
}

void ExamRoomManagement::onLogoutClicked()
{
    UserData::instance().reset();
    emit logout();
}