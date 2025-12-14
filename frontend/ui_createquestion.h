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
#include <QtWidgets/QPushButton>
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
    QHBoxLayout *answerHeaderLayout;
    QLabel *answersLabel;
    QSpacerItem *horizontalSpacer2;
    QPushButton *addAnswerButton;
    QWidget *answerContainer;
    QPushButton *saveButton;

    void setupUi(QWidget *CreateQuestion)
    {
        if (CreateQuestion->objectName().isEmpty())
            CreateQuestion->setObjectName(QString::fromUtf8("CreateQuestion"));
        CreateQuestion->resize(650, 500);
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

        answerHeaderLayout = new QHBoxLayout();
        answerHeaderLayout->setObjectName(QString::fromUtf8("answerHeaderLayout"));
        answersLabel = new QLabel(CreateQuestion);
        answersLabel->setObjectName(QString::fromUtf8("answersLabel"));
        answersLabel->setFont(font1);

        answerHeaderLayout->addWidget(answersLabel);

        horizontalSpacer2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        answerHeaderLayout->addItem(horizontalSpacer2);

        addAnswerButton = new QPushButton(CreateQuestion);
        addAnswerButton->setObjectName(QString::fromUtf8("addAnswerButton"));
        addAnswerButton->setCursor(QCursor(Qt::PointingHandCursor));

        answerHeaderLayout->addWidget(addAnswerButton);


        verticalLayout->addLayout(answerHeaderLayout);

        answerContainer = new QWidget(CreateQuestion);
        answerContainer->setObjectName(QString::fromUtf8("answerContainer"));
        answerContainer->setMinimumSize(QSize(0, 200));

        verticalLayout->addWidget(answerContainer);

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

        answersLabel->setText(QCoreApplication::translate("CreateQuestion", "C\303\241c \304\221\303\241p \303\241n:", nullptr));
        addAnswerButton->setText(QCoreApplication::translate("CreateQuestion", "+ Th\303\252m \304\221\303\241p \303\241n", nullptr));
        saveButton->setText(QCoreApplication::translate("CreateQuestion", "L\306\260u c\303\242u h\341\273\217i", nullptr));
        saveButton->setStyleSheet(QCoreApplication::translate("CreateQuestion", "background-color: #6600FF; color: white; padding: 8px;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateQuestion: public Ui_CreateQuestion {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEQUESTION_H
