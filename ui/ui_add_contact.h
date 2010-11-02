/********************************************************************************
** Form generated from reading UI file 'add_contactFW3252.ui'
**
** Created: Sun May 16 10:03:46 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef ADD_CONTACTFW3252_H
#define ADD_CONTACTFW3252_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_XfireAddContact
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *lblUsername;
    QLineEdit *prefUsername;
    QPlainTextEdit *prefMessage;
    QLabel *lblInvitationMessage;

    void setupUi(QWidget *XfireAddContact)
    {
        if (XfireAddContact->objectName().isEmpty())
            XfireAddContact->setObjectName(QString::fromUtf8("XfireAddContact"));
        XfireAddContact->resize(496, 277);
        verticalLayout = new QVBoxLayout(XfireAddContact);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        lblUsername = new QLabel(XfireAddContact);
        lblUsername->setObjectName(QString::fromUtf8("lblUsername"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lblUsername);

        prefUsername = new QLineEdit(XfireAddContact);
        prefUsername->setObjectName(QString::fromUtf8("prefUsername"));

        formLayout->setWidget(0, QFormLayout::FieldRole, prefUsername);

        prefMessage = new QPlainTextEdit(XfireAddContact);
        prefMessage->setObjectName(QString::fromUtf8("prefMessage"));

        formLayout->setWidget(1, QFormLayout::FieldRole, prefMessage);

        lblInvitationMessage = new QLabel(XfireAddContact);
        lblInvitationMessage->setObjectName(QString::fromUtf8("lblInvitationMessage"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lblInvitationMessage);


        verticalLayout->addLayout(formLayout);


        retranslateUi(XfireAddContact);

        QMetaObject::connectSlotsByName(XfireAddContact);
    } // setupUi

    void retranslateUi(QWidget *XfireAddContact)
    {
        XfireAddContact->setWindowTitle(QApplication::translate("XfireAddContact", "Form", 0, QApplication::UnicodeUTF8));
        lblUsername->setText(QApplication::translate("XfireAddContact", "Xfire username:", 0, QApplication::UnicodeUTF8));
        prefMessage->setPlainText(QApplication::translate("XfireAddContact", "Please add me to your friends list!", 0, QApplication::UnicodeUTF8));
        lblInvitationMessage->setText(QApplication::translate("XfireAddContact", "Invitation message:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XfireAddContact: public Ui_XfireAddContact {};
} // namespace Ui

QT_END_NAMESPACE

#endif // ADD_CONTACTFW3252_H
