/********************************************************************************
** Form generated from reading UI file 'createexamroom.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEEXAMROOM_H
#define UI_CREATEEXAMROOM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CreateExamRoom
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QFormLayout *formLayout;
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QLabel *descLabel;
    QLineEdit *descriptionLineEdit;
    QLabel *easyLabel;
    QSpinBox *easySpinBox;
    QLabel *mediumLabel;
    QSpinBox *mediumSpinBox;
    QLabel *hardLabel;
    QSpinBox *hardSpinBox;
    QLabel *timeLimitLabel;
    QSpinBox *timeLimitSpinBox;
    QLabel *startLabel;
    QDateTimeEdit *startDateTimeEdit;
    QLabel *endLabel;
    QDateTimeEdit *endDateTimeEdit;
    QSpacerItem *verticalSpacer;
    QPushButton *saveButton;

    void setupUi(QWidget *CreateExamRoom)
    {
        if (CreateExamRoom->objectName().isEmpty())
            CreateExamRoom->setObjectName(QString::fromUtf8("CreateExamRoom"));
        CreateExamRoom->resize(500, 450);
        verticalLayout = new QVBoxLayout(CreateExamRoom);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(CreateExamRoom);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        nameLabel = new QLabel(CreateExamRoom);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        QFont font1;
        font1.setBold(true);
        nameLabel->setFont(font1);

        formLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);

        nameLineEdit = new QLineEdit(CreateExamRoom);
        nameLineEdit->setObjectName(QString::fromUtf8("nameLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, nameLineEdit);

        descLabel = new QLabel(CreateExamRoom);
        descLabel->setObjectName(QString::fromUtf8("descLabel"));
        descLabel->setFont(font1);

        formLayout->setWidget(1, QFormLayout::LabelRole, descLabel);

        descriptionLineEdit = new QLineEdit(CreateExamRoom);
        descriptionLineEdit->setObjectName(QString::fromUtf8("descriptionLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, descriptionLineEdit);

        easyLabel = new QLabel(CreateExamRoom);
        easyLabel->setObjectName(QString::fromUtf8("easyLabel"));
        easyLabel->setFont(font1);

        formLayout->setWidget(2, QFormLayout::LabelRole, easyLabel);

        easySpinBox = new QSpinBox(CreateExamRoom);
        easySpinBox->setObjectName(QString::fromUtf8("easySpinBox"));
        easySpinBox->setMinimum(0);
        easySpinBox->setMaximum(100);

        formLayout->setWidget(2, QFormLayout::FieldRole, easySpinBox);

        mediumLabel = new QLabel(CreateExamRoom);
        mediumLabel->setObjectName(QString::fromUtf8("mediumLabel"));
        mediumLabel->setFont(font1);

        formLayout->setWidget(3, QFormLayout::LabelRole, mediumLabel);

        mediumSpinBox = new QSpinBox(CreateExamRoom);
        mediumSpinBox->setObjectName(QString::fromUtf8("mediumSpinBox"));
        mediumSpinBox->setMinimum(0);
        mediumSpinBox->setMaximum(100);

        formLayout->setWidget(3, QFormLayout::FieldRole, mediumSpinBox);

        hardLabel = new QLabel(CreateExamRoom);
        hardLabel->setObjectName(QString::fromUtf8("hardLabel"));
        hardLabel->setFont(font1);

        formLayout->setWidget(4, QFormLayout::LabelRole, hardLabel);

        hardSpinBox = new QSpinBox(CreateExamRoom);
        hardSpinBox->setObjectName(QString::fromUtf8("hardSpinBox"));
        hardSpinBox->setMinimum(0);
        hardSpinBox->setMaximum(100);

        formLayout->setWidget(4, QFormLayout::FieldRole, hardSpinBox);

        timeLimitLabel = new QLabel(CreateExamRoom);
        timeLimitLabel->setObjectName(QString::fromUtf8("timeLimitLabel"));
        timeLimitLabel->setFont(font1);

        formLayout->setWidget(5, QFormLayout::LabelRole, timeLimitLabel);

        timeLimitSpinBox = new QSpinBox(CreateExamRoom);
        timeLimitSpinBox->setObjectName(QString::fromUtf8("timeLimitSpinBox"));
        timeLimitSpinBox->setMinimum(1);
        timeLimitSpinBox->setMaximum(300);
        timeLimitSpinBox->setValue(60);

        formLayout->setWidget(5, QFormLayout::FieldRole, timeLimitSpinBox);

        startLabel = new QLabel(CreateExamRoom);
        startLabel->setObjectName(QString::fromUtf8("startLabel"));
        startLabel->setFont(font1);

        formLayout->setWidget(6, QFormLayout::LabelRole, startLabel);

        startDateTimeEdit = new QDateTimeEdit(CreateExamRoom);
        startDateTimeEdit->setObjectName(QString::fromUtf8("startDateTimeEdit"));
        startDateTimeEdit->setCalendarPopup(true);

        formLayout->setWidget(6, QFormLayout::FieldRole, startDateTimeEdit);

        endLabel = new QLabel(CreateExamRoom);
        endLabel->setObjectName(QString::fromUtf8("endLabel"));
        endLabel->setFont(font1);

        formLayout->setWidget(7, QFormLayout::LabelRole, endLabel);

        endDateTimeEdit = new QDateTimeEdit(CreateExamRoom);
        endDateTimeEdit->setObjectName(QString::fromUtf8("endDateTimeEdit"));
        endDateTimeEdit->setCalendarPopup(true);

        formLayout->setWidget(7, QFormLayout::FieldRole, endDateTimeEdit);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        saveButton = new QPushButton(CreateExamRoom);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout->addWidget(saveButton);


        retranslateUi(CreateExamRoom);

        QMetaObject::connectSlotsByName(CreateExamRoom);
    } // setupUi

    void retranslateUi(QWidget *CreateExamRoom)
    {
        CreateExamRoom->setWindowTitle(QCoreApplication::translate("CreateExamRoom", "T\341\272\241o Ph\303\262ng Thi", nullptr));
        titleLabel->setText(QCoreApplication::translate("CreateExamRoom", "T\341\272\241o Ph\303\262ng Thi M\341\273\233i", nullptr));
        nameLabel->setText(QCoreApplication::translate("CreateExamRoom", "T\303\252n ph\303\262ng thi:", nullptr));
        nameLineEdit->setPlaceholderText(QCoreApplication::translate("CreateExamRoom", "Nh\341\272\255p t\303\252n ph\303\262ng thi", nullptr));
        descLabel->setText(QCoreApplication::translate("CreateExamRoom", "M\303\264 t\341\272\243:", nullptr));
        descriptionLineEdit->setPlaceholderText(QCoreApplication::translate("CreateExamRoom", "Nh\341\272\255p m\303\264 t\341\272\243", nullptr));
        easyLabel->setText(QCoreApplication::translate("CreateExamRoom", "S\341\273\221 c\303\242u d\341\273\205:", nullptr));
        mediumLabel->setText(QCoreApplication::translate("CreateExamRoom", "S\341\273\221 c\303\242u trung b\303\254nh:", nullptr));
        hardLabel->setText(QCoreApplication::translate("CreateExamRoom", "S\341\273\221 c\303\242u kh\303\263:", nullptr));
        timeLimitLabel->setText(QCoreApplication::translate("CreateExamRoom", "Th\341\273\235i gian (ph\303\272t):", nullptr));
        startLabel->setText(QCoreApplication::translate("CreateExamRoom", "Th\341\273\235i gian b\341\272\257t \304\221\341\272\247u:", nullptr));
        startDateTimeEdit->setDisplayFormat(QCoreApplication::translate("CreateExamRoom", "dd/MM/yyyy HH:mm", nullptr));
        endLabel->setText(QCoreApplication::translate("CreateExamRoom", "Th\341\273\235i gian k\341\272\277t th\303\272c:", nullptr));
        endDateTimeEdit->setDisplayFormat(QCoreApplication::translate("CreateExamRoom", "dd/MM/yyyy HH:mm", nullptr));
        saveButton->setText(QCoreApplication::translate("CreateExamRoom", "L\306\260u", nullptr));
        saveButton->setStyleSheet(QCoreApplication::translate("CreateExamRoom", "background-color: #6600FF; color: white; padding: 8px;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateExamRoom: public Ui_CreateExamRoom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEEXAMROOM_H
