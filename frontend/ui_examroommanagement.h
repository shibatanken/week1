/********************************************************************************
** Form generated from reading UI file 'examroommanagement.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXAMROOMMANAGEMENT_H
#define UI_EXAMROOMMANAGEMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExamRoomManagement
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *headerLayout;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *userNameLabel;
    QPushButton *logoutButton;
    QHBoxLayout *actionLayout;
    QSpacerItem *horizontalSpacer2;
    QPushButton *createRoomButton;
    QTableWidget *roomTableWidget;

    void setupUi(QWidget *ExamRoomManagement)
    {
        if (ExamRoomManagement->objectName().isEmpty())
            ExamRoomManagement->setObjectName(QString::fromUtf8("ExamRoomManagement"));
        ExamRoomManagement->resize(800, 600);
        verticalLayout = new QVBoxLayout(ExamRoomManagement);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName(QString::fromUtf8("headerLayout"));
        titleLabel = new QLabel(ExamRoomManagement);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);

        headerLayout->addWidget(titleLabel);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        headerLayout->addItem(horizontalSpacer);

        userNameLabel = new QLabel(ExamRoomManagement);
        userNameLabel->setObjectName(QString::fromUtf8("userNameLabel"));

        headerLayout->addWidget(userNameLabel);

        logoutButton = new QPushButton(ExamRoomManagement);
        logoutButton->setObjectName(QString::fromUtf8("logoutButton"));
        logoutButton->setCursor(QCursor(Qt::PointingHandCursor));

        headerLayout->addWidget(logoutButton);


        verticalLayout->addLayout(headerLayout);

        actionLayout = new QHBoxLayout();
        actionLayout->setObjectName(QString::fromUtf8("actionLayout"));
        horizontalSpacer2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        actionLayout->addItem(horizontalSpacer2);

        createRoomButton = new QPushButton(ExamRoomManagement);
        createRoomButton->setObjectName(QString::fromUtf8("createRoomButton"));
        createRoomButton->setCursor(QCursor(Qt::PointingHandCursor));

        actionLayout->addWidget(createRoomButton);


        verticalLayout->addLayout(actionLayout);

        roomTableWidget = new QTableWidget(ExamRoomManagement);
        roomTableWidget->setObjectName(QString::fromUtf8("roomTableWidget"));
        roomTableWidget->setCursor(QCursor(Qt::PointingHandCursor));
        roomTableWidget->setAlternatingRowColors(true);
        roomTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(roomTableWidget);


        retranslateUi(ExamRoomManagement);

        QMetaObject::connectSlotsByName(ExamRoomManagement);
    } // setupUi

    void retranslateUi(QWidget *ExamRoomManagement)
    {
        ExamRoomManagement->setWindowTitle(QCoreApplication::translate("ExamRoomManagement", "Qu\341\272\243n l\303\275 Ph\303\262ng Thi", nullptr));
        titleLabel->setText(QCoreApplication::translate("ExamRoomManagement", "\360\237\223\213 Qu\341\272\243n l\303\275 Ph\303\262ng Thi", nullptr));
        userNameLabel->setText(QCoreApplication::translate("ExamRoomManagement", "Xin ch\303\240o", nullptr));
        logoutButton->setText(QCoreApplication::translate("ExamRoomManagement", "\304\220\304\203ng xu\341\272\245t", nullptr));
        createRoomButton->setText(QCoreApplication::translate("ExamRoomManagement", "+ T\341\272\241o ph\303\262ng thi m\341\273\233i", nullptr));
        createRoomButton->setStyleSheet(QCoreApplication::translate("ExamRoomManagement", "background-color: #6600FF; color: white; padding: 8px 16px;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExamRoomManagement: public Ui_ExamRoomManagement {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXAMROOMMANAGEMENT_H
