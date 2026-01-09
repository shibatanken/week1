/********************************************************************************
** Form generated from reading UI file 'home.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOME_H
#define UI_HOME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Home
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QSpacerItem *verticalSpacer_1;
    QLabel *welcomeLabel;
    QLabel *emailLabel;
    QSpacerItem *verticalSpacer_2;
    QPushButton *examRoomButton;
    QSpacerItem *verticalSpacer_3;
    QPushButton *logoutButton;

    void setupUi(QWidget *Home)
    {
        if (Home->objectName().isEmpty())
            Home->setObjectName("Home");
        Home->resize(500, 400);
        verticalLayout = new QVBoxLayout(Home);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(Home);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        verticalSpacer_1 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_1);

        welcomeLabel = new QLabel(Home);
        welcomeLabel->setObjectName("welcomeLabel");
        QFont font1;
        font1.setPointSize(16);
        welcomeLabel->setFont(font1);
        welcomeLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(welcomeLabel);

        emailLabel = new QLabel(Home);
        emailLabel->setObjectName("emailLabel");
        emailLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(emailLabel);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        examRoomButton = new QPushButton(Home);
        examRoomButton->setObjectName("examRoomButton");
        examRoomButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(examRoomButton);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        logoutButton = new QPushButton(Home);
        logoutButton->setObjectName("logoutButton");
        logoutButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(logoutButton);


        retranslateUi(Home);

        QMetaObject::connectSlotsByName(Home);
    } // setupUi

    void retranslateUi(QWidget *Home)
    {
        Home->setWindowTitle(QCoreApplication::translate("Home", "Trang ch\341\273\247", nullptr));
        titleLabel->setText(QCoreApplication::translate("Home", "\360\237\217\240 Trang Ch\341\273\247 - Quiz App", nullptr));
        welcomeLabel->setText(QCoreApplication::translate("Home", "Ch\303\240o m\341\273\253ng!", nullptr));
        emailLabel->setText(QCoreApplication::translate("Home", "Email:", nullptr));
        examRoomButton->setText(QCoreApplication::translate("Home", "\360\237\223\213 Qu\341\272\243n l\303\275 Ph\303\262ng Thi", nullptr));
        examRoomButton->setStyleSheet(QCoreApplication::translate("Home", "background-color: #6600FF; color: white; padding: 12px; font-size: 14px;", nullptr));
        logoutButton->setText(QCoreApplication::translate("Home", "\304\220\304\203ng xu\341\272\245t", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Home: public Ui_Home {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOME_H
