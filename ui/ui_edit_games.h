/********************************************************************************
** Form generated from reading UI file 'edit_gamesEf2659.ui'
**
** Created: Mon May 17 20:53:05 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef EDIT_GAMESEF2659_H
#define EDIT_GAMESEF2659_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include "kurlrequester.h"

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *treeWidget;
    QPushButton *btnConfigureAutomaitcally;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *enableCheckBox;
    QFormLayout *formLayout;
    QLabel *label;
    KUrlRequester *detectionRequester;
    QLabel *label_2;
    KUrlRequester *launchRequester;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(824, 463);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        treeWidget = new QTreeWidget(Dialog);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        treeWidget->setMinimumSize(QSize(0, 0));
        treeWidget->setMaximumSize(QSize(350, 16777215));
        treeWidget->header()->setVisible(true);

        verticalLayout_3->addWidget(treeWidget);

        btnConfigureAutomaitcally = new QPushButton(Dialog);
        btnConfigureAutomaitcally->setObjectName(QString::fromUtf8("btnConfigureAutomaitcally"));

        verticalLayout_3->addWidget(btnConfigureAutomaitcally);


        horizontalLayout->addLayout(verticalLayout_3);

        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        enableCheckBox = new QCheckBox(groupBox);
        enableCheckBox->setObjectName(QString::fromUtf8("enableCheckBox"));

        verticalLayout_2->addWidget(enableCheckBox);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        detectionRequester = new KUrlRequester(groupBox);
        detectionRequester->setObjectName(QString::fromUtf8("detectionRequester"));
        detectionRequester->setEnabled(false);
        detectionRequester->setMinimumSize(QSize(220, 0));

        formLayout->setWidget(1, QFormLayout::FieldRole, detectionRequester);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(false);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        launchRequester = new KUrlRequester(groupBox);
        launchRequester->setObjectName(QString::fromUtf8("launchRequester"));
        launchRequester->setEnabled(false);
        launchRequester->setMinimumSize(QSize(220, 0));

        formLayout->setWidget(2, QFormLayout::FieldRole, launchRequester);


        verticalLayout_2->addLayout(formLayout);

        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        buttonBox = new QDialogButtonBox(groupBox);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Reset);

        verticalLayout_2->addWidget(buttonBox);


        horizontalLayout->addWidget(groupBox);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Configure games", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("Dialog", "Games", 0, QApplication::UnicodeUTF8));
        btnConfigureAutomaitcally->setText(QApplication::translate("Dialog", "Configure games automatically", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Dialog", "Game settings", 0, QApplication::UnicodeUTF8));
        enableCheckBox->setText(QApplication::translate("Dialog", "Enable detection for this game", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Detection executable:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "Launch executable:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EDIT_GAMESEF2659_H
