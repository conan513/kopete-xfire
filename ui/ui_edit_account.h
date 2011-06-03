/********************************************************************************
** Form generated from reading UI file 'edit_accountql2707.ui'
**
** Created: Fri Jun 3 10:50:21 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef EDIT_ACCOUNTQL2707_H
#define EDIT_ACCOUNTQL2707_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "klineedit.h"
#include "kopetepasswordwidget.h"

QT_BEGIN_NAMESPACE

class Ui_XfireEditAccount
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabPreferences;
    QWidget *tabBasic;
    QVBoxLayout *vboxLayout;
    QGroupBox *gpbAccount;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hblAccount;
    QLabel *lblUsername;
    QLineEdit *prefUsername;
    Kopete::UI::PasswordWidget *prefPassword;
    QCheckBox *prefExcludeConnect;
    QGroupBox *gpbRegistration;
    QVBoxLayout *verticalLayout_2;
    QLabel *lblRegistration;
    QHBoxLayout *hblRegistration;
    QSpacerItem *hspRegistrationLeft;
    QPushButton *pbtRegister;
    QSpacerItem *hspRegistrationRight;
    QWidget *tabGeneral;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *gpbGeneral;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *prefGameDetection;
    QCheckBox *prefInformAccounts;
    QCheckBox *prefShowClanFriends;
    QCheckBox *prefFriendsOfFriends;
    QCheckBox *prefPeerToPeer;
    QSpacerItem *vspGeneral;
    QWidget *tabConnection;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *gpbConnection;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *prefOverrideServer;
    QWidget *wdgOverrideServer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *hspOverrideServer;
    QLabel *lblServerPort;
    KLineEdit *prefServer;
    QSpinBox *prefPort;
    QGroupBox *gpbVersion;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lblVersion;
    QSpinBox *prefVersion;
    QSpacerItem *horizontalSpacer;
    QCheckBox *prefUpdateVersion;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *XfireEditAccount)
    {
        if (XfireEditAccount->objectName().isEmpty())
            XfireEditAccount->setObjectName(QString::fromUtf8("XfireEditAccount"));
        XfireEditAccount->resize(491, 287);
        verticalLayout = new QVBoxLayout(XfireEditAccount);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabPreferences = new QTabWidget(XfireEditAccount);
        tabPreferences->setObjectName(QString::fromUtf8("tabPreferences"));
        tabPreferences->setMinimumSize(QSize(460, 0));
        tabBasic = new QWidget();
        tabBasic->setObjectName(QString::fromUtf8("tabBasic"));
        vboxLayout = new QVBoxLayout(tabBasic);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gpbAccount = new QGroupBox(tabBasic);
        gpbAccount->setObjectName(QString::fromUtf8("gpbAccount"));
        vboxLayout1 = new QVBoxLayout(gpbAccount);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        hblAccount = new QHBoxLayout();
#ifndef Q_OS_MAC
        hblAccount->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hblAccount->setContentsMargins(0, 0, 0, 0);
#endif
        hblAccount->setObjectName(QString::fromUtf8("hblAccount"));
        lblUsername = new QLabel(gpbAccount);
        lblUsername->setObjectName(QString::fromUtf8("lblUsername"));

        hblAccount->addWidget(lblUsername);

        prefUsername = new QLineEdit(gpbAccount);
        prefUsername->setObjectName(QString::fromUtf8("prefUsername"));

        hblAccount->addWidget(prefUsername);


        vboxLayout1->addLayout(hblAccount);

        prefPassword = new Kopete::UI::PasswordWidget(gpbAccount);
        prefPassword->setObjectName(QString::fromUtf8("prefPassword"));
        prefPassword->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(prefPassword->sizePolicy().hasHeightForWidth());
        prefPassword->setSizePolicy(sizePolicy);

        vboxLayout1->addWidget(prefPassword);

        prefExcludeConnect = new QCheckBox(gpbAccount);
        prefExcludeConnect->setObjectName(QString::fromUtf8("prefExcludeConnect"));
        prefExcludeConnect->setChecked(false);

        vboxLayout1->addWidget(prefExcludeConnect);


        vboxLayout->addWidget(gpbAccount);

        gpbRegistration = new QGroupBox(tabBasic);
        gpbRegistration->setObjectName(QString::fromUtf8("gpbRegistration"));
        verticalLayout_2 = new QVBoxLayout(gpbRegistration);
#ifndef Q_OS_MAC
        verticalLayout_2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        verticalLayout_2->setContentsMargins(9, 9, 9, 9);
#endif
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lblRegistration = new QLabel(gpbRegistration);
        lblRegistration->setObjectName(QString::fromUtf8("lblRegistration"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblRegistration->sizePolicy().hasHeightForWidth());
        lblRegistration->setSizePolicy(sizePolicy1);
        lblRegistration->setMinimumSize(QSize(0, 0));
        lblRegistration->setWordWrap(true);

        verticalLayout_2->addWidget(lblRegistration);

        hblRegistration = new QHBoxLayout();
        hblRegistration->setObjectName(QString::fromUtf8("hblRegistration"));
        hspRegistrationLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hblRegistration->addItem(hspRegistrationLeft);

        pbtRegister = new QPushButton(gpbRegistration);
        pbtRegister->setObjectName(QString::fromUtf8("pbtRegister"));

        hblRegistration->addWidget(pbtRegister);

        hspRegistrationRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hblRegistration->addItem(hspRegistrationRight);


        verticalLayout_2->addLayout(hblRegistration);


        vboxLayout->addWidget(gpbRegistration);

        tabPreferences->addTab(tabBasic, QString());
        tabGeneral = new QWidget();
        tabGeneral->setObjectName(QString::fromUtf8("tabGeneral"));
        horizontalLayout_2 = new QHBoxLayout(tabGeneral);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        gpbGeneral = new QGroupBox(tabGeneral);
        gpbGeneral->setObjectName(QString::fromUtf8("gpbGeneral"));
        gpbGeneral->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        verticalLayout_4 = new QVBoxLayout(gpbGeneral);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        prefGameDetection = new QCheckBox(gpbGeneral);
        prefGameDetection->setObjectName(QString::fromUtf8("prefGameDetection"));
        prefGameDetection->setEnabled(true);
        prefGameDetection->setChecked(true);

        verticalLayout_4->addWidget(prefGameDetection);

        prefInformAccounts = new QCheckBox(gpbGeneral);
        prefInformAccounts->setObjectName(QString::fromUtf8("prefInformAccounts"));

        verticalLayout_4->addWidget(prefInformAccounts);

        prefShowClanFriends = new QCheckBox(gpbGeneral);
        prefShowClanFriends->setObjectName(QString::fromUtf8("prefShowClanFriends"));

        verticalLayout_4->addWidget(prefShowClanFriends);

        prefFriendsOfFriends = new QCheckBox(gpbGeneral);
        prefFriendsOfFriends->setObjectName(QString::fromUtf8("prefFriendsOfFriends"));
        prefFriendsOfFriends->setChecked(true);

        verticalLayout_4->addWidget(prefFriendsOfFriends);

        prefPeerToPeer = new QCheckBox(gpbGeneral);
        prefPeerToPeer->setObjectName(QString::fromUtf8("prefPeerToPeer"));

        verticalLayout_4->addWidget(prefPeerToPeer);

        vspGeneral = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(vspGeneral);


        horizontalLayout_2->addWidget(gpbGeneral);

        tabPreferences->addTab(tabGeneral, QString());
        tabConnection = new QWidget();
        tabConnection->setObjectName(QString::fromUtf8("tabConnection"));
        verticalLayout_3 = new QVBoxLayout(tabConnection);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gpbConnection = new QGroupBox(tabConnection);
        gpbConnection->setObjectName(QString::fromUtf8("gpbConnection"));
        verticalLayout_5 = new QVBoxLayout(gpbConnection);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        prefOverrideServer = new QCheckBox(gpbConnection);
        prefOverrideServer->setObjectName(QString::fromUtf8("prefOverrideServer"));

        verticalLayout_5->addWidget(prefOverrideServer);

        wdgOverrideServer = new QWidget(gpbConnection);
        wdgOverrideServer->setObjectName(QString::fromUtf8("wdgOverrideServer"));
        wdgOverrideServer->setEnabled(true);
        horizontalLayout = new QHBoxLayout(wdgOverrideServer);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        hspOverrideServer = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(hspOverrideServer);

        lblServerPort = new QLabel(wdgOverrideServer);
        lblServerPort->setObjectName(QString::fromUtf8("lblServerPort"));
        lblServerPort->setEnabled(false);

        horizontalLayout->addWidget(lblServerPort);

        prefServer = new KLineEdit(wdgOverrideServer);
        prefServer->setObjectName(QString::fromUtf8("prefServer"));
        prefServer->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(prefServer->sizePolicy().hasHeightForWidth());
        prefServer->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(prefServer);

        prefPort = new QSpinBox(wdgOverrideServer);
        prefPort->setObjectName(QString::fromUtf8("prefPort"));
        prefPort->setEnabled(false);
        prefPort->setMinimum(1);
        prefPort->setMaximum(65535);
        prefPort->setValue(25999);

        horizontalLayout->addWidget(prefPort);


        verticalLayout_5->addWidget(wdgOverrideServer);


        verticalLayout_3->addWidget(gpbConnection);

        gpbVersion = new QGroupBox(tabConnection);
        gpbVersion->setObjectName(QString::fromUtf8("gpbVersion"));
        verticalLayout_6 = new QVBoxLayout(gpbVersion);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lblVersion = new QLabel(gpbVersion);
        lblVersion->setObjectName(QString::fromUtf8("lblVersion"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lblVersion->sizePolicy().hasHeightForWidth());
        lblVersion->setSizePolicy(sizePolicy3);

        horizontalLayout_3->addWidget(lblVersion);

        prefVersion = new QSpinBox(gpbVersion);
        prefVersion->setObjectName(QString::fromUtf8("prefVersion"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(prefVersion->sizePolicy().hasHeightForWidth());
        prefVersion->setSizePolicy(sizePolicy4);
        prefVersion->setMaximum(999);

        horizontalLayout_3->addWidget(prefVersion);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_6->addLayout(horizontalLayout_3);

        prefUpdateVersion = new QCheckBox(gpbVersion);
        prefUpdateVersion->setObjectName(QString::fromUtf8("prefUpdateVersion"));
        prefUpdateVersion->setChecked(true);

        verticalLayout_6->addWidget(prefUpdateVersion);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);


        verticalLayout_3->addWidget(gpbVersion);

        tabPreferences->addTab(tabConnection, QString());

        verticalLayout->addWidget(tabPreferences);

#ifndef QT_NO_SHORTCUT
        lblUsername->setBuddy(prefUsername);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(tabPreferences, prefUsername);
        QWidget::setTabOrder(prefUsername, prefExcludeConnect);
        QWidget::setTabOrder(prefExcludeConnect, pbtRegister);
        QWidget::setTabOrder(pbtRegister, prefGameDetection);
        QWidget::setTabOrder(prefGameDetection, prefInformAccounts);
        QWidget::setTabOrder(prefInformAccounts, prefOverrideServer);
        QWidget::setTabOrder(prefOverrideServer, prefServer);
        QWidget::setTabOrder(prefServer, prefPort);

        retranslateUi(XfireEditAccount);

        tabPreferences->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(XfireEditAccount);
    } // setupUi

    void retranslateUi(QWidget *XfireEditAccount)
    {
        XfireEditAccount->setWindowTitle(QApplication::translate("XfireEditAccount", "Account preferences - Xfire", 0, QApplication::UnicodeUTF8));
        gpbAccount->setTitle(QApplication::translate("XfireEditAccount", "Account information", 0, QApplication::UnicodeUTF8));
        lblUsername->setText(QApplication::translate("XfireEditAccount", "&Xfire username:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        prefUsername->setToolTip(QApplication::translate("XfireEditAccount", "The account name of your Yahoo account.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        prefUsername->setWhatsThis(QApplication::translate("XfireEditAccount", "The account name of your Yahoo account.  This should be in the form of an alphanumeric string (no spaces).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        prefExcludeConnect->setWhatsThis(QApplication::translate("XfireEditAccount", "If you check this checkbox, the account will not be connected when you press the \"Connect All\" button, or at startup when automatic connection at startup is enabled.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        prefExcludeConnect->setText(QApplication::translate("XfireEditAccount", "Exclude from connect all", 0, QApplication::UnicodeUTF8));
        gpbRegistration->setTitle(QApplication::translate("XfireEditAccount", "Registration", 0, QApplication::UnicodeUTF8));
        lblRegistration->setText(QApplication::translate("XfireEditAccount", "If you do not currently have a Xfire account, please click the connect the button to create one in order to connect.", 0, QApplication::UnicodeUTF8));
        pbtRegister->setText(QApplication::translate("XfireEditAccount", "Register new account", 0, QApplication::UnicodeUTF8));
        tabPreferences->setTabText(tabPreferences->indexOf(tabBasic), QApplication::translate("XfireEditAccount", "&Basic", 0, QApplication::UnicodeUTF8));
        gpbGeneral->setTitle(QApplication::translate("XfireEditAccount", "General preferences", 0, QApplication::UnicodeUTF8));
        prefGameDetection->setText(QApplication::translate("XfireEditAccount", "Use game detection", 0, QApplication::UnicodeUTF8));
        prefInformAccounts->setText(QApplication::translate("XfireEditAccount", "Inform other accounts about ingame status", 0, QApplication::UnicodeUTF8));
        prefShowClanFriends->setText(QApplication::translate("XfireEditAccount", "Show clan friends", 0, QApplication::UnicodeUTF8));
        prefFriendsOfFriends->setText(QApplication::translate("XfireEditAccount", "Show friends of friends", 0, QApplication::UnicodeUTF8));
        prefPeerToPeer->setText(QApplication::translate("XfireEditAccount", "Use peer to peer capabilities", 0, QApplication::UnicodeUTF8));
        tabPreferences->setTabText(tabPreferences->indexOf(tabGeneral), QApplication::translate("XfireEditAccount", "&General", 0, QApplication::UnicodeUTF8));
        gpbConnection->setTitle(QApplication::translate("XfireEditAccount", "Connection preferences", 0, QApplication::UnicodeUTF8));
        prefOverrideServer->setText(QApplication::translate("XfireEditAccount", "Override default server information", 0, QApplication::UnicodeUTF8));
        lblServerPort->setText(QApplication::translate("XfireEditAccount", "Server / port:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        prefServer->setToolTip(QApplication::translate("XfireEditAccount", "Only modify these values if you want to use a special IM proxy server, like SIMP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        prefServer->setWhatsThis(QApplication::translate("XfireEditAccount", "Only modify these values if you want to use a special IM proxy server, like SIMP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        prefServer->setText(QApplication::translate("XfireEditAccount", "cs.xfire.com", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        prefPort->setToolTip(QApplication::translate("XfireEditAccount", "Only modify these values if you want to use a special IM proxy server, like SIMP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        prefPort->setWhatsThis(QApplication::translate("XfireEditAccount", "Only modify these values if you want to use a special IM proxy server, like SIMP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        gpbVersion->setTitle(QApplication::translate("XfireEditAccount", "Xfire version", 0, QApplication::UnicodeUTF8));
        lblVersion->setText(QApplication::translate("XfireEditAccount", "Version:", 0, QApplication::UnicodeUTF8));
        prefUpdateVersion->setText(QApplication::translate("XfireEditAccount", "Update version automatically", 0, QApplication::UnicodeUTF8));
        tabPreferences->setTabText(tabPreferences->indexOf(tabConnection), QApplication::translate("XfireEditAccount", "&Connection", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XfireEditAccount: public Ui_XfireEditAccount {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EDIT_ACCOUNTQL2707_H
