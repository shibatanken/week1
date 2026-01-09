#include "questionbank.h"
#include "config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>

QuestionBank::QuestionBank(int classId, bool selectionMode, QWidget *parent)
    : QDialog(parent), m_classId(classId), m_selectionMode(selectionMode), m_selectedQuestionId(-1)
{
    setupUi();
    loadQuestions();
}

QuestionBank::~QuestionBank() {}

int QuestionBank::getSelectedQuestionId() const {
    return m_selectedQuestionId;
}

void QuestionBank::setupUi() {
    this->setWindowTitle(m_selectionMode ? "Chọn câu hỏi từ Ngân hàng" : "Ngân hàng câu hỏi");
    this->resize(600, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_listWidget = new QListWidget(this);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString details = QString("Câu hỏi: %1\n\nA. %2\nB. %3\nC. %4\nD. %5\n\nĐáp án đúng: %6")
            .arg(item->text())
            .arg(item->data(Qt::UserRole + 1).toString())
            .arg(item->data(Qt::UserRole + 2).toString())
            .arg(item->data(Qt::UserRole + 3).toString())
            .arg(item->data(Qt::UserRole + 4).toString())
            .arg(item->data(Qt::UserRole + 5).toString());
        QMessageBox::information(this, "Chi tiết câu hỏi", details);
    });
    mainLayout->addWidget(m_listWidget);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_refreshBtn = new QPushButton("Làm mới", this);
    connect(m_refreshBtn, &QPushButton::clicked, this, &QuestionBank::loadQuestions);
    btnLayout->addWidget(m_refreshBtn);

    if (m_selectionMode) {
        m_selectBtn = new QPushButton("Chọn câu này", this);
        connect(m_selectBtn, &QPushButton::clicked, this, &QuestionBank::onSelectQuestionClicked);
        btnLayout->addWidget(m_selectBtn);
    } else {
        m_addBtn = new QPushButton("Thêm câu hỏi mới", this);
        connect(m_addBtn, &QPushButton::clicked, this, &QuestionBank::onAddQuestionClicked);
        btnLayout->addWidget(m_addBtn);

        m_deleteBtn = new QPushButton("Xóa câu hỏi", this);
        connect(m_deleteBtn, &QPushButton::clicked, this, &QuestionBank::onDeleteQuestionClicked);
        btnLayout->addWidget(m_deleteBtn);
    }

    QPushButton *closeBtn = new QPushButton("Đóng", this);
    connect(closeBtn, &QPushButton::clicked, this, &QuestionBank::reject);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);
}

void QuestionBank::loadQuestions() {
    m_listWidget->clear();
    m_listWidget->addItem("Đang tải...");
    
    // Dùng async để tránh block UI
    QTcpSocket *socket = new QTcpSocket(this);
    
    connect(socket, &QTcpSocket::connected, this, [this, socket]() {
        QJsonObject json;
        json["class_id"] = m_classId;
        QJsonDocument doc(json);
        QString req = QString("CONTROL GET_QUESTION_BANK\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
        socket->write(req.toUtf8());
        socket->flush();
    });
    
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        QByteArray resp = socket->readAll();
        QString respStr(resp);
        
        m_listWidget->clear();
        
        int idx = respStr.indexOf('{');
        if (idx != -1) {
            QJsonDocument d = QJsonDocument::fromJson(respStr.mid(idx).toUtf8());
            QJsonArray arr = d.object()["data"].toArray();
            
            if (arr.isEmpty()) {
                m_listWidget->addItem("(Không có câu hỏi nào)");
            }
            
            for (const auto &val : arr) {
                QJsonObject obj = val.toObject();
                QString content = obj["content"].toString();
                int id = obj["id"].toInt();
                QString optA = obj["option_a"].toString();
                QString optB = obj["option_b"].toString();
                QString optC = obj["option_c"].toString();
                QString optD = obj["option_d"].toString();
                QString correct = obj["correct_option"].toString();
                
                QListWidgetItem *item = new QListWidgetItem(content + " (ID: " + QString::number(id) + ")");
                item->setData(Qt::UserRole, id);
                item->setData(Qt::UserRole + 1, optA);
                item->setData(Qt::UserRole + 2, optB);
                item->setData(Qt::UserRole + 3, optC);
                item->setData(Qt::UserRole + 4, optD);
                item->setData(Qt::UserRole + 5, correct);
                m_listWidget->addItem(item);
            }
        }
        
        socket->close();
        socket->deleteLater();
    });
    
    connect(socket, &QTcpSocket::errorOccurred, this, [this, socket](QAbstractSocket::SocketError) {
        m_listWidget->clear();
        m_listWidget->addItem("(Lỗi kết nối server)");
        socket->deleteLater();
    });
    
    socket->connectToHost(IPADDRESS, 8081);
}

void QuestionBank::onAddQuestionClicked() {
    showAddQuestionDialog();
}

void QuestionBank::showAddQuestionDialog() {
    QDialog dlg(this);
    dlg.setWindowTitle("Thêm câu hỏi mới");
    dlg.resize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    
    QTextEdit *contentEdit = new QTextEdit();
    contentEdit->setPlaceholderText("Nội dung câu hỏi...");
    layout->addWidget(contentEdit);
    
    QLineEdit *optA = new QLineEdit(); optA->setPlaceholderText("Đáp án A"); layout->addWidget(optA);
    QLineEdit *optB = new QLineEdit(); optB->setPlaceholderText("Đáp án B"); layout->addWidget(optB);
    QLineEdit *optC = new QLineEdit(); optC->setPlaceholderText("Đáp án C"); layout->addWidget(optC);
    QLineEdit *optD = new QLineEdit(); optD->setPlaceholderText("Đáp án D"); layout->addWidget(optD);
    
    QGroupBox *grp = new QGroupBox("Đáp án đúng");
    QHBoxLayout *h = new QHBoxLayout(grp);
    QRadioButton *rA = new QRadioButton("A"); h->addWidget(rA);
    QRadioButton *rB = new QRadioButton("B"); h->addWidget(rB);
    QRadioButton *rC = new QRadioButton("C"); h->addWidget(rC);
    QRadioButton *rD = new QRadioButton("D"); h->addWidget(rD);
    rA->setChecked(true);
    layout->addWidget(grp);
    
    QPushButton *saveBtn = new QPushButton("Lưu");
    layout->addWidget(saveBtn);
    
    connect(saveBtn, &QPushButton::clicked, [&]() {
        if(contentEdit->toPlainText().isEmpty() || optA->text().isEmpty() || 
           optB->text().isEmpty() || optC->text().isEmpty() || optD->text().isEmpty()) {
            QMessageBox::warning(&dlg, "Lỗi", "Vui lòng nhập đủ thông tin");
            return;
        }

        QJsonObject json;
        json["class_id"] = m_classId;
        json["content"] = contentEdit->toPlainText();
        json["option_a"] = optA->text();
        json["option_b"] = optB->text();
        json["option_c"] = optC->text();
        json["option_d"] = optD->text();
        
        if(rA->isChecked()) json["correct_option"] = "A";
        else if(rB->isChecked()) json["correct_option"] = "B";
        else if(rC->isChecked()) json["correct_option"] = "C";
        else json["correct_option"] = "D";
        
        QJsonDocument doc(json);
        QString req = QString("CONTROL ADD_QUESTION_TO_BANK\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
        
        QTcpSocket socket;
        socket.connectToHost(IPADDRESS, 8081);
        if(socket.waitForConnected(3000)) {
            socket.write(req.toUtf8());
            socket.flush();
            if(socket.waitForReadyRead(3000)) {
                 QString res = QString(socket.readAll());
                 if(res.contains("SUCCESS")) {
                     QMessageBox::information(&dlg, "OK", "Đã thêm câu hỏi");
                     dlg.accept();
                     loadQuestions();
                 } else {
                     QMessageBox::warning(&dlg, "Lỗi", res);
                 }
            }
        }
    });
    
    dlg.exec();
}

void QuestionBank::onDeleteQuestionClicked() {
    QListWidgetItem *item = m_listWidget->currentItem();
    if(!item) {
         QMessageBox::warning(this, "Lỗi", "Chọn câu hỏi cần xóa");
         return;
    }
    
    int qId = item->data(Qt::UserRole).toInt();
    if(QMessageBox::question(this, "Xác nhận", "Xóa câu hỏi này?") != QMessageBox::Yes) return;
    
    QJsonObject json;
    json["question_id"] = qId;
    QJsonDocument doc(json);
    QString req = QString("CONTROL DELETE_QUESTION_FROM_BANK\n%1").arg(QString(doc.toJson(QJsonDocument::Compact)));
     
    QTcpSocket socket;
    socket.connectToHost(IPADDRESS, 8081);
    if(socket.waitForConnected(3000)) {
        socket.write(req.toUtf8());
        socket.flush();
        socket.waitForReadyRead(1000);
        loadQuestions();
    }
}

void QuestionBank::onSelectQuestionClicked() {
    QListWidgetItem *item = m_listWidget->currentItem();
    if(!item) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một câu hỏi");
        return;
    }
    m_selectedQuestionId = item->data(Qt::UserRole).toInt();
    emit questionSelected(m_selectedQuestionId);
    accept();
}
