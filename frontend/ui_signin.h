/********************************************************************************
** Form generated from reading UI file 'signin.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNIN_H
#define UI_SIGNIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Signin
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QPushButton *signinButton;
    QPushButton *signupButton;
    QLabel *responseLabel;

    void setupUi(QWidget *Signin)
    {
        if (Signin->objectName().isEmpty())
            Signin->setObjectName("Signin");
        Signin->resize(400, 300);
        verticalLayout = new QVBoxLayout(Signin);
        verticalLayout->setObjectName("verticalLayout");
        emailLabel = new QLabel(Signin);
        emailLabel->setObjectName("emailLabel");

        verticalLayout->addWidget(emailLabel);

        emailLineEdit = new QLineEdit(Signin);
        emailLineEdit->setObjectName("emailLineEdit");

        verticalLayout->addWidget(emailLineEdit);

        passwordLabel = new QLabel(Signin);
        passwordLabel->setObjectName("passwordLabel");

        verticalLayout->addWidget(passwordLabel);

        passwordLineEdit = new QLineEdit(Signin);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(passwordLineEdit);

        signinButton = new QPushButton(Signin);
        signinButton->setObjectName("signinButton");
        signinButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout->addWidget(signinButton);

        signupButton = new QPushButton(Signin);
        signupButton->setObjectName("signupButton");

        verticalLayout->addWidget(signupButton);

        responseLabel = new QLabel(Signin);
        responseLabel->setObjectName("responseLabel");

        verticalLayout->addWidget(responseLabel);


        retranslateUi(Signin);

        QMetaObject::connectSlotsByName(Signin);
    } // setupUi

    void retranslateUi(QWidget *Signin)
    {
        Signin->setWindowTitle(QCoreApplication::translate("Signin", "Signin", nullptr));
        emailLabel->setText(QCoreApplication::translate("Signin", "Email", nullptr));
        passwordLabel->setText(QCoreApplication::translate("Signin", "Password", nullptr));
        signinButton->setText(QCoreApplication::translate("Signin", "Signin", nullptr));
        signupButton->setText(QCoreApplication::translate("Signin", "Signup", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Signin: public Ui_Signin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNIN_H
