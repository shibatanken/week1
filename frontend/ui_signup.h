/********************************************************************************
** Form generated from reading UI file 'signup.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUP_H
#define UI_SIGNUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Signup
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *dobLabel;
    QDateTimeEdit *dateTimeEdit;
    QPushButton *signupButton;
    QPushButton *signinButton;
    QLabel *dobLineEdit;

    void setupUi(QWidget *Signup)
    {
        if (Signup->objectName().isEmpty())
            Signup->setObjectName("Signup");
        Signup->resize(400, 300);
        verticalLayout = new QVBoxLayout(Signup);
        verticalLayout->setObjectName("verticalLayout");
        emailLabel = new QLabel(Signup);
        emailLabel->setObjectName("emailLabel");

        verticalLayout->addWidget(emailLabel);

        emailLineEdit = new QLineEdit(Signup);
        emailLineEdit->setObjectName("emailLineEdit");

        verticalLayout->addWidget(emailLineEdit);

        passwordLabel = new QLabel(Signup);
        passwordLabel->setObjectName("passwordLabel");

        verticalLayout->addWidget(passwordLabel);

        passwordLineEdit = new QLineEdit(Signup);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(passwordLineEdit);

        usernameLabel = new QLabel(Signup);
        usernameLabel->setObjectName("usernameLabel");

        verticalLayout->addWidget(usernameLabel);

        usernameLineEdit = new QLineEdit(Signup);
        usernameLineEdit->setObjectName("usernameLineEdit");

        verticalLayout->addWidget(usernameLineEdit);

        dobLabel = new QLabel(Signup);
        dobLabel->setObjectName("dobLabel");

        verticalLayout->addWidget(dobLabel);

        dateTimeEdit = new QDateTimeEdit(Signup);
        dateTimeEdit->setObjectName("dateTimeEdit");

        verticalLayout->addWidget(dateTimeEdit);

        signupButton = new QPushButton(Signup);
        signupButton->setObjectName("signupButton");
        signupButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(signupButton);

        signinButton = new QPushButton(Signup);
        signinButton->setObjectName("signinButton");
        signinButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(signinButton);

        dobLineEdit = new QLabel(Signup);
        dobLineEdit->setObjectName("dobLineEdit");

        verticalLayout->addWidget(dobLineEdit);


        retranslateUi(Signup);

        QMetaObject::connectSlotsByName(Signup);
    } // setupUi

    void retranslateUi(QWidget *Signup)
    {
        Signup->setWindowTitle(QCoreApplication::translate("Signup", "Signup", nullptr));
        emailLabel->setText(QCoreApplication::translate("Signup", "Email", nullptr));
        passwordLabel->setText(QCoreApplication::translate("Signup", "Password", nullptr));
        usernameLabel->setText(QCoreApplication::translate("Signup", "Username", nullptr));
        dobLabel->setText(QCoreApplication::translate("Signup", "Dob", nullptr));
        signupButton->setText(QCoreApplication::translate("Signup", "Signup", nullptr));
        signinButton->setText(QCoreApplication::translate("Signup", "Signin", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Signup: public Ui_Signup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUP_H
