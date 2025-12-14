/********************************************************************************
** Form generated from reading UI file 'examroomdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXAMROOMDIALOG_H
#define UI_EXAMROOMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExamRoomDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *roomNameLabel;
    QLabel *descriptionLabel;
    QLabel *timeLimitLabel;
    QFrame *line;
    QHBoxLayout *questionHeaderLayout;
    QLabel *questionBankLabel;
    QLabel *questionCountLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *addQuestionButton;
    QListWidget *questionListWidget;

    void setupUi(QWidget *ExamRoomDialog)
    {
        if (ExamRoomDialog->objectName().isEmpty())
            ExamRoomDialog->setObjectName(QString::fromUtf8("ExamRoomDialog"));
        ExamRoomDialog->resize(700, 500);
        verticalLayout = new QVBoxLayout(ExamRoomDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        roomNameLabel = new QLabel(ExamRoomDialog);
        roomNameLabel->setObjectName(QString::fromUtf8("roomNameLabel"));
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        font.setWeight(75);
        roomNameLabel->setFont(font);

        verticalLayout->addWidget(roomNameLabel);

        descriptionLabel = new QLabel(ExamRoomDialog);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));

        verticalLayout->addWidget(descriptionLabel);

        timeLimitLabel = new QLabel(ExamRoomDialog);
        timeLimitLabel->setObjectName(QString::fromUtf8("timeLimitLabel"));

        verticalLayout->addWidget(timeLimitLabel);

        line = new QFrame(ExamRoomDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        questionHeaderLayout = new QHBoxLayout();
        questionHeaderLayout->setObjectName(QString::fromUtf8("questionHeaderLayout"));
        questionBankLabel = new QLabel(ExamRoomDialog);
        questionBankLabel->setObjectName(QString::fromUtf8("questionBankLabel"));
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setWeight(75);
        questionBankLabel->setFont(font1);

        questionHeaderLayout->addWidget(questionBankLabel);

        questionCountLabel = new QLabel(ExamRoomDialog);
        questionCountLabel->setObjectName(QString::fromUtf8("questionCountLabel"));

        questionHeaderLayout->addWidget(questionCountLabel);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        questionHeaderLayout->addItem(horizontalSpacer);

        addQuestionButton = new QPushButton(ExamRoomDialog);
        addQuestionButton->setObjectName(QString::fromUtf8("addQuestionButton"));
        addQuestionButton->setCursor(QCursor(Qt::PointingHandCursor));

        questionHeaderLayout->addWidget(addQuestionButton);


        verticalLayout->addLayout(questionHeaderLayout);

        questionListWidget = new QListWidget(ExamRoomDialog);
        questionListWidget->setObjectName(QString::fromUtf8("questionListWidget"));
        questionListWidget->setAlternatingRowColors(true);

        verticalLayout->addWidget(questionListWidget);


        retranslateUi(ExamRoomDialog);

        QMetaObject::connectSlotsByName(ExamRoomDialog);
    } // setupUi

    void retranslateUi(QWidget *ExamRoomDialog)
    {
        ExamRoomDialog->setWindowTitle(QCoreApplication::translate("ExamRoomDialog", "Chi ti\341\272\277t Ph\303\262ng Thi", nullptr));
        roomNameLabel->setText(QCoreApplication::translate("ExamRoomDialog", "T\303\252n ph\303\262ng thi", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("ExamRoomDialog", "M\303\264 t\341\272\243", nullptr));
        timeLimitLabel->setText(QCoreApplication::translate("ExamRoomDialog", "Th\341\273\235i gian:", nullptr));
        questionBankLabel->setText(QCoreApplication::translate("ExamRoomDialog", "\360\237\223\232 Ng\303\242n h\303\240ng c\303\242u h\341\273\217i", nullptr));
        questionCountLabel->setText(QCoreApplication::translate("ExamRoomDialog", "S\341\273\221 c\303\242u h\341\273\217i: 0", nullptr));
        addQuestionButton->setText(QCoreApplication::translate("ExamRoomDialog", "+ Th\303\252m c\303\242u h\341\273\217i", nullptr));
        addQuestionButton->setStyleSheet(QCoreApplication::translate("ExamRoomDialog", "background-color: #6600FF; color: white; padding: 6px 12px;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExamRoomDialog: public Ui_ExamRoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXAMROOMDIALOG_H
