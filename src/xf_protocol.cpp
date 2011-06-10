/**
* Copyright 2010  Warren Dumortier <nwarrenfl@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <KGenericFactory>

#include "kopeteappearancesettings.h"

#include "xf_account.h"
#include "xf_add_contact.h"
#include "xf_edit_account.h"
#include "xf_protocol.h"

XfireProtocol *XfireProtocol::m_protocol = 0;

K_PLUGIN_FACTORY(XfireProtocolFactory, registerPlugin<XfireProtocol>(););
K_EXPORT_PLUGIN(XfireProtocolFactory("kopete_xfire"));

XfireProtocol::XfireProtocol(QObject *parent, const QVariantList &)
    : Kopete::Protocol(XfireProtocolFactory::componentData(), parent, true),
    XfireOffline(Kopete::OnlineStatus::Offline, 0, this, 0, QStringList(), i18n("Offline"), i18n("Offline"), Kopete::OnlineStatusManager::Offline),
    XfireConnecting(Kopete::OnlineStatus::Connecting, 1, this, 2, QStringList(QString::fromUtf8("xfire_connecting")), i18n("Connecting"), i18n("Connecting"), 0, Kopete::OnlineStatusManager::HideFromMenu),
    XfireOnline(Kopete::OnlineStatus::Online, 2, this, 1, QStringList(), i18n("Online"), i18n("Online"), Kopete::OnlineStatusManager::Online, Kopete::OnlineStatusManager::HasStatusMessage),
    XfireAway(Kopete::OnlineStatus::Away, 3, this, 1, QStringList(), i18n("Away"), i18n("Away"), Kopete::OnlineStatusManager::Away, Kopete::OnlineStatusManager::HasStatusMessage),
    propGame("currentGame", i18n("Game"), QString(), Kopete::PropertyTmpl::PrivateProperty),
    propServer("currentServer", i18n("Server"), QString(), Kopete::PropertyTmpl::PrivateProperty)
{
    // Load protocol only once
    if(m_protocol)
    {
        kDebug() << "Protocol already loaded, aborting initialization";
        return;
    }

    kDebug() << "Protocol loaded";
    m_protocol = this;

    addAddressBookField("messaging/xfire", Kopete::Plugin::MakeIndexField);

    // Contact properties
    QStringList shownProps = Kopete::AppearanceSettings::self()->toolTipContents();
    if(!shownProps.contains("currentGame"))
        shownProps << QString::fromLatin1("currentGame");

    if(!shownProps.contains("currentServer"))
        shownProps << QString::fromLatin1("currentServer");

    Kopete::AppearanceSettings::self()->setToolTipContents(shownProps);
}

XfireProtocol::~XfireProtocol()
{
    m_protocol = 0;
}

AddContactPage *XfireProtocol::createAddContactWidget(QWidget *parent, Kopete::Account *account)
{
    kDebug() << "Creating add contact widget";
    return new XfireAddContactPage(account, parent);
}

KopeteEditAccountWidget *XfireProtocol::createEditAccountWidget(Kopete::Account *account, QWidget *parent)
{
    kDebug() << "Creating edit account widget";
    return new XfireEditAccountWidget(parent, account);
}

Kopete::Account* XfireProtocol::createNewAccount(const QString &accountId)
{
    kDebug() << "Creating new account:" << accountId;

    // Don't create an account twice
    if(Kopete::AccountManager::self()->findAccount(pluginId(), accountId))
        return 0;

    return new XfireAccount(this, accountId);
}

XfireProtocol* XfireProtocol::protocol()
{
    return m_protocol;
}

Kopete::Contact* XfireProtocol::deserializeContact(Kopete::MetaContact *metaContact, const QMap<QString, QString> &serializedData, const QMap<QString, QString> &)
{
    QString contactId = serializedData["contactId"];
    QString accountId = serializedData["accountId"];

    XfireAccount *thisAccount = static_cast<XfireAccount*>(Kopete::AccountManager::self()->findAccount(pluginId(), accountId));
    if(!thisAccount)
        return 0;

    if(thisAccount->findContact(contactId))
    {
        kDebug() << contactId << "already in contacts map";
        return 0;
    }

    thisAccount->addContact(contactId, metaContact, Kopete::Account::DontChangeKABC);
    return thisAccount->contacts().value(contactId);
}
