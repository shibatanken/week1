/********************************************************************************
** Form generated from reading UI file 'createquestion.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEQUESTION_H
#define UI_CREATEQUESTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CreateQuestion
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *questionLabel;
    QTextEdit *questionTextEdit;
    QHBoxLayout *difficultyLayout;
    QLabel *difficultyLabel;
    QComboBox *difficultyComboBox;
    QSpacerItem *horizontalSpacer;
    QLabel *answersLabel;
    QHBoxLayout *optionALayout;
    QRadioButton *radioA;
    QLineEdit *optionA;
    QHBoxLayout *optionBLayout;
    QRadioButton *radioB;
    QLineEdit *optionB;
    QHBoxLayout *optionCLayout;
    QRadioButton *radioC;
    QLineEdit *optionC;
    QHBoxLayout *optionDLayout;
    QRadioButton *radioD;
    QLineEdit *optionD;
    QPushButton *saveButton;

    void setupUi(QWidget *CreateQuestion)
    {
        if (CreateQuestion->objectName().isEmpty())
            CreateQuestion->setObjectName(QString::fromUtf8("CreateQuestion"));
        CreateQuestion->resize(650, 600);
        verticalLayout = new QVBoxLayout(CreateQuestion);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(CreateQuestion);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        questionLabel = new QLabel(CreateQuestion);
        questionLabel->setObjectName(QString::fromUtf8("questionLabel"));
        QFont font1;
        font1.setBold(true);
        questionLabel->setFont(font1);

        verticalLayout->addWidget(questionLabel);

        questionTextEdit = new QTextEdit(CreateQuestion);
        questionTextEdit->setObjectName(QString::fromUtf8("questionTextEdit"));
        questionTextEdit->setMinimumSize(QSize(0, 80));

        verticalLayout->addWidget(questionTextEdit);

        difficultyLayout = new QHBoxLayout();
        difficultyLayout->setObjectName(QString::fromUtf8("difficultyLayout"));
        difficultyLabel = new QLabel(CreateQuestion);
        difficultyLabel->setObjectName(QString::fromUtf8("difficultyLabel"));
        difficultyLabel->setFont(font1);

        difficultyLayout->addWidget(difficultyLabel);

        difficultyComboBox = new QComboBox(CreateQuestion);
        difficultyComboBox->addItem(QString());
        difficultyComboBox->addItem(QString());
        difficultyComboBox->addItem(QString());
        difficultyComboBox->setObjectName(QString::fromUtf8("difficultyComboBox"));

        difficultyLayout->addWidget(difficultyComboBox);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        difficultyLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(difficultyLayout);

        answersLabel = new QLabel(CreateQuestion);
        answersLabel->setObjectName(QString::fromUtf8("answersLabel"));
        answersLabel->setFont(font1);

        verticalLayout->addWidget(answersLabel);

        optionALayout = new QHBoxLayout();
        optionALayout->setObjectName(QString::fromUtf8("optionALayout"));
        radioA = new QRadioButton(CreateQuestion);
        radioA->setObjectName(QString::fromUtf8("radioA"));
        radioA->setChecked(true);

        optionALayout->addWidget(radioA);

        optionA = new QLineEdit(CreateQuestion);
        optionA->setObjectName(QString::fromUtf8("optionA"));

        optionALayout->addWidget(optionA);


        verticalLayout->addLayout(optionALayout);

        optionBLayout = new QHBoxLayout();
        optionBLayout->setObjectName(QString::fromUtf8("optionBLayout"));
        radioB = new QRadioButton(CreateQuestion);
        radioB->setObjectName(QString::fromUtf8("radioB"));

        optionBLayout->addWidget(radioB);

        optionB = new QLineEdit(CreateQuestion);
        optionB->setObjectName(QString::fromUtf8("optionB"));

        optionBLayout->addWidget(optionB);


        verticalLayout->addLayout(optionBLayout);

        optionCLayout = new QHBoxLayout();
        optionCLayout->setObjectName(QString::fromUtf8("optionCLayout"));
        radioC = new QRadioButton(CreateQuestion);
        radioC->setObjectName(QString::fromUtf8("radioC"));

        optionCLayout->addWidget(radioC);

        optionC = new QLineEdit(CreateQuestion);
        optionC->setObjectName(QString::fromUtf8("optionC"));

        optionCLayout->addWidget(optionC);


        verticalLayout->addLayout(optionCLayout);

        optionDLayout = new QHBoxLayout();
        optionDLayout->setObjectName(QString::fromUtf8("optionDLayout"));
        radioD = new QRadioButton(CreateQuestion);
        radioD->setObjectName(QString::fromUtf8("radioD"));

        optionDLayout->addWidget(radioD);

        optionD = new QLineEdit(CreateQuestion);
        optionD->setObjectName(QString::fromUtf8("optionD"));

        optionDLayout->addWidget(optionD);


        verticalLayout->addLayout(optionDLayout);

        saveButton = new QPushButton(CreateQuestion);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout->addWidget(saveButton);


        retranslateUi(CreateQuestion);

        QMetaObject::connectSlotsByName(CreateQuestion);
    } // setupUi

    void retranslateUi(QWidget *CreateQuestion)
    {
        CreateQuestion->setWindowTitle(QCoreApplication::translate("CreateQuestion", "T\341\272\241o C\303\242u H\341\273\217i", nullptr));
        titleLabel->setText(QCoreApplication::translate("CreateQuestion", "T\341\272\241o C\303\242u H\341\273\217i M\341\273\233i", nullptr));
        questionLabel->setText(QCoreApplication::translate("CreateQuestion", "N\341\273\231i dung c\303\242u h\341\273\217i:", nullptr));
        questionTextEdit->setPlaceholderText(QCoreApplication::translate("CreateQuestion", "Nh\341\272\255p n\341\273\231i dung c\303\242u h\341\273\217i...", nullptr));
        difficultyLabel->setText(QCoreApplication::translate("CreateQuestion", "\304\220\341\273\231 kh\303\263:", nullptr));
        difficultyComboBox->setItemText(0, QCoreApplication::translate("CreateQuestion", "D\341\273\205", nullptr));
        difficultyComboBox->setItemText(1, QCoreApplication::translate("CreateQuestion", "Trung b\303\254nh", nullptr));
        difficultyComboBox->setItemText(2, QCoreApplication::translate("CreateQuestion", "Kh\303\263", nullptr));

        answersLabel->setText(QCoreApplication::translate("CreateQuestion", "C\303\241c \304\221\303\241p \303\241n (Ch\341\273\215n \304\221\303\241p \303\241n \304\221\303\272ng):", nullptr));
        radioA->setText(QCoreApplication::translate("CreateQuestion", "A", nullptr));
        optionA->setPlaceholderText(QCoreApplication::translate("CreateQuestion", "Nh\341\272\255p \304\221\303\241p \303\241n A", nullptr));
        radioB->setText(QCoreApplication::translate("CreateQuestion", "B", nullptr));
        optionB->setPlaceholderText(QCoreApplication::translate("CreateQuestion", "Nh\341\272\255p \304\221\303\241p \303\241n B", nullptr));
        radioC->setText(QCoreApplication::translate("CreateQuestion", "C", nullptr));
        optionC->setPlaceholderText(QCoreApplication::translate("CreateQuestion", "Nh\341\272\255p \304\221\303\241p \303\241n C", nullptr));
        radioD->setText(QCoreApplication::translate("CreateQuestion", "D", nullptr));
        optionD->setPlaceholderText(QCoreApplication::translate("CreateQuestion", "Nh\341\272\255p \304\221\303\241p \303\241n D", nullptr));
        saveButton->setText(QCoreApplication::translate("CreateQuestion", "L\306\260u c\303\242u h\341\273\217i", nullptr));
        saveButton->setStyleSheet(QCoreApplication::translate("CreateQuestion", "background-color: #6600FF; color: white; padding: 8px;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateQuestion: public Ui_CreateQuestion {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEQUESTION_H
