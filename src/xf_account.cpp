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

#include <KAction>
#include <KActionMenu>
#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KMenu>
#include <KNotification>

#include <kopeteaccount.h>
#include <kopeteaddedinfoevent.h>
#include <kopetecontact.h>
#include <kopetecontactlist.h>
#include <kopetegroup.h>
#include <kopetemetacontact.h>
#include <kopetepasswordedaccount.h>

#include "xf_account.h"
#include "xf_contact.h"
#include "xf_game_detection.h"
#include "xf_games_list.h"
#include "xf_games_manager.h"
#include "xf_p2p.h"
#include "xf_p2p_session.h"
#include "xf_protocol.h"
#include "xf_server.h"

XfireAccount::XfireAccount(XfireProtocol *parent, const QString &accountId)
    : Kopete::PasswordedAccount(parent, accountId),
    m_gamesList(new XfireGamesList()),
    m_gamesDetection(0),
    m_p2pConnection(0),
    m_server(new XfireServer(this)),
    m_gamesManager(new XfireGamesManager(this))
{
    // Create the myself contact and set the initial status to offline
    setMyself(new XfireContact(this, accountId, accountId, Kopete::ContactList::self()->myself()));
    myself()->setOnlineStatus(parent->XfireOffline);

    m_openGamesManager = new KAction(KIcon("input-gaming"), i18n("Configure games"), this);
    QObject::connect(m_openGamesManager, SIGNAL(triggered()), this, SLOT(slotOpenGamesManager()));

    QObject::connect(m_gamesList, SIGNAL(gamesListReady()), this, SLOT(slotContinueConnecting()));
    QObject::connect(m_gamesList, SIGNAL(gamesListReady()), m_gamesManager, SLOT(slotUpdate()));
}

XfireAccount::~XfireAccount()
{
    delete m_gamesManager;
    delete m_gamesDetection;
    delete m_p2pConnection;
}

void XfireAccount::fillActionMenu(KActionMenu *p_actionMenu)
{
    Kopete::Account::fillActionMenu(p_actionMenu);

    m_openGamesManager->setEnabled(true);
    p_actionMenu->addSeparator();
    p_actionMenu->addAction(m_openGamesManager);
}

void XfireAccount::connectWithPassword(const QString &p_password)
{
    // Cancel connection if no password has been supplied
    if(p_password.isEmpty())
    {
        emit slotGoOffline();
        return;
    }

    // Don't attempt to connect when already online
    if(myself()->onlineStatus() != XfireProtocol::protocol()->XfireOffline)
        return;

    m_password = p_password;
    m_gamesList->slotUpdate();
}

void XfireAccount::slotContinueConnecting()
{
    kDebug() << "Starting login sequence";

    // Make new connection to the Xfire server
    m_server->connectToServer(accountId(), m_password, serverName(), serverPort());

    // Initialize game detection if enabled
    if(isGameDetectionEnabled())
    {
        m_gamesDetection = new XfireGameDetection(this);
        QObject::connect(m_gamesDetection, SIGNAL(gameRunning()), this, SLOT(slotSendIngameStatus()));
        m_gamesDetection->start();
    }

    // Initialize peer to peer if enabled
    if(isPeerToPeerEnabled())
        m_p2pConnection = new XfireP2P(this);
}

void XfireAccount::slotSendIngameStatus()
{
    m_server->sendIngameStatus(m_gamesDetection->m_currentGame.id, m_gamesDetection->m_currentGame.ip, m_gamesDetection->m_currentGame.port);

    // Inform other Kopete accounts (Xfire: Game)
    if(isInformAccountsEnabled())
    {
        QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts();
        foreach(Kopete::Account *a, accounts)
        {
            if(a->protocol() == myself()->protocol())
                continue;

            Kopete::StatusMessage message;
            if(m_gamesDetection->m_currentGame.id == 0)
                message.setMessage("");
            else
                message.setMessage("Xfire: " + m_gamesList->getGameNameFromID(m_gamesDetection->m_currentGame.id));

            a->setStatusMessage(message);
        }
    }
}

void XfireAccount::logOff(Kopete::Account::DisconnectReason p_reason)
{
    kDebug() << "Disconnecting from the Xfire server";

    m_server->closeConnection();
    myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOffline);

    if(p_reason == Kopete::Account::OtherClient)
        kDebug() << "Logged in somewhere else";

    disconnected(p_reason);
}

void XfireAccount::disconnect()
{
    logOff(Kopete::Account::Manual);
}

bool XfireAccount::createContact(const QString &p_contactId, Kopete::MetaContact *p_parentContact)
{
    XfireContact *newContact = new XfireContact(this, p_contactId, p_parentContact->displayName(), p_parentContact);
    return newContact != 0;
}

void XfireAccount::setStatusMessage(const Kopete::StatusMessage &p_statusMessage)
{
    if(isConnected() && myself()->statusMessage().message() != p_statusMessage.message())
    {
        kDebug() << "Setting status message:" << p_statusMessage.message();
        m_server->sendStatusMessage(p_statusMessage.message());
    }
}

void XfireAccount::slotGoOnline()
{
    if(!isConnected())
        connect(XfireProtocol::protocol()->XfireConnecting);
    else
    {
        myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
        server()->sendStatusMessage(QString());
    }
}

void XfireAccount::slotGoAway()
{
    if(!isConnected())
        connect(XfireProtocol::protocol()->XfireConnecting);

    myself()->setOnlineStatus(XfireProtocol::protocol()->XfireAway);
    server()->sendStatusMessage(i18n("(AFK) Away From Keyboard"));
}

void XfireAccount::slotGoOffline()
{
    if(isConnected() || myself()->onlineStatus().status() == Kopete::OnlineStatus::Connecting)
        disconnect();
}

void XfireAccount::newContact(const QString &p_contactId, const QString &p_name, int p_groupId)
{
    Kopete::Group *group;
    if(p_groupId != -1)
        group = Kopete::ContactList::self()->group(p_groupId);
    else
        group = Kopete::ContactList::self()->findGroup("Xfire");

    XfireContact *contact = static_cast<XfireContact *>(contacts().value(p_contactId));
    if(!contact)
        addContact(p_contactId, p_name, group);
    else
        contact->metaContact()->addToGroup(group);
}

void XfireAccount::updateContactID(const QString &p_contactId, Xfire::Int32Attribute *p_id)
{
    XfireContact*contact = static_cast<XfireContact* >(contacts().value(p_contactId));
    if(contact)
        contact->setId(p_id);
}

void XfireAccount::updateContactSID(Xfire::Int32Attribute *p_id, Xfire::SIDAttribute *p_sid)
{
    XfireContact* contact = (XfireContact* )findContact(p_id->value());
    if(contact)
    {
        if(contact->m_id == p_id->value())
        {
            // Update the SID of the contact
            contact->m_sid = p_sid->sid();

            // Set the online status of the contact
            if(p_sid->sid().isValid())
                contact->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
            else
                contact->setOnlineStatus(XfireProtocol::protocol()->XfireOffline);
        }
    }
}

void XfireAccount::updateContactGameInformation(const Xfire::SessionID &p_sid, quint32 p_gameId, quint32 p_serverIp, quint16 p_serverPort)
{
    XfireContact *c = static_cast<XfireContact*>(findContact(p_sid));
    if(c)
    {
        c->removeProperties();

        if(p_serverIp && p_serverPort)
            c->setProperty(XfireProtocol::protocol()->propServer, QHostAddress(p_serverIp).toString() + ":" + QString::number(p_serverPort));

        QString message;
        QString gameName = m_gamesList->getGameNameFromID(p_gameId);

        if(!p_gameId)
            message = "";
        else if(gameName == 0L)
            message = "Playing " + QString::number(p_gameId);
        else
            message = "Playing " + gameName;

        if(gameName != 0L)
            c->setProperty(XfireProtocol::protocol()->propGame, gameName);

        Kopete::StatusMessage status(message);
        c->setStatusMessage(status);
    }
}

void XfireAccount::setStatus(const Xfire::SessionID &p_sid, const QString &p_statusMessage)
{
    XfireContact *c = static_cast<XfireContact*>(findContact(p_sid));
    if(c)
    {
        Kopete::StatusMessage status(p_statusMessage);
        c->setStatusMessage(status);

        if(p_statusMessage.contains("(AFK)"))
            c->setOnlineStatus(XfireProtocol::protocol()->XfireAway);
        else if(c->Away)
            c->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
    }
}

Kopete::Contact* XfireAccount::findContact(quint32 p_id)
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for(i = hash.begin(); i != hash.end(); ++i)
    {
        XfireContact *contact = static_cast<XfireContact*>(i.value());
        if(contact->m_id == p_id)
            return i.value();
    }

    return 0; // Contact not found
}


Kopete::Contact* XfireAccount::findContact(const Xfire::SessionID &p_sid)
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for(i = hash.begin(); i != hash.end(); ++i)
    {
        XfireContact *contact = static_cast<XfireContact*>(i.value());
        if(contact->m_sid == p_sid)
            return i.value();
    }

    return 0; // Contact not found
}

Kopete::Contact* XfireAccount::findContact(QString p_contactId)
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for(i = hash.begin(); i != hash.end(); ++i)
    {
        XfireContact *contact = static_cast<XfireContact*>(i.value());
        if(contact->contactId() == p_contactId)
            return i.value();
    }

    return 0; // Contact not found
}

QString XfireAccount::serverName() const
{
    if(configGroup()->readEntry("Custom_server", false))
        return configGroup()->readEntry("ServerIP", XFIRE_SERVER_IP);
    else
        return XFIRE_SERVER_IP;
}

uint XfireAccount::serverPort() const
{

    if(configGroup()->readEntry("Custom_server", false))
        return configGroup()->readEntry("ServerPort", XFIRE_SERVER_PORT);
    else
        return XFIRE_SERVER_PORT;
}

uint XfireAccount::protocolVersion() const
{
    if(configGroup()->readEntry("ProtocolVersion", false))
        return configGroup()->readEntry("ProtocolVersion", XFIRE_PROTO_VERSION);
    else
        return XFIRE_PROTO_VERSION;
}

bool XfireAccount::isGameDetectionEnabled()
{
    return configGroup()->readEntry("GameDetection", true);
}

bool XfireAccount::isInformAccountsEnabled()
{
    return configGroup()->readEntry("InformAccounts", false);
}

bool XfireAccount::isFriendsOfFriendsEnabled()
{
    return configGroup()->readEntry("FriendsOfFriends", true);
}

bool XfireAccount::isPeerToPeerEnabled()
{
    return configGroup()->readEntry("PeerToPeer", false);
}

bool XfireAccount::isClanFriendsEnabled()
{
    return configGroup()->readEntry("ClanFriends", false);
}

void XfireAccount::setOnlineStatus(const Kopete::OnlineStatus &p_status, const Kopete::StatusMessage &p_reason, const OnlineStatusOptions &p_options)
{
    Q_UNUSED(p_options);

    setStatusMessage(p_reason.message());

    kDebug() << "Setting online status to:" << p_status.description();

    if(p_status == XfireProtocol::protocol()->XfireConnecting && myself()->onlineStatus() == XfireProtocol::protocol()->XfireOffline)
        slotGoOnline();
    else if(p_status == XfireProtocol::protocol()->XfireOnline || p_status == Kopete::OnlineStatus::Online)
        slotGoOnline();
    else if(p_status == XfireProtocol::protocol()->XfireOffline || p_status == Kopete::OnlineStatus::Offline)
        slotGoOffline();
    else if(p_status == XfireProtocol::protocol()->XfireAway || p_status == Kopete::OnlineStatus::Away)
        slotGoAway();
    else if(p_status == Kopete::OnlineStatus::Invisible)
        slotGoOnline();
}

void XfireAccount::changeOurStatus(const Kopete::OnlineStatus &p_status)
{
    kDebug() << "Changing status to:" << p_status.description();

    if(p_status == XfireProtocol::protocol()->XfireOnline)
        myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
}

XfireServer* XfireAccount::server()
{
    return m_server;
}

XfireP2PSession* XfireAccount::p2pSessionByMoniker(QByteArray p_moniker)
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for(i = hash.begin(); i != hash.end(); ++i)
    {
        XfireContact *contact = static_cast<XfireContact*>(i.value());
        if(contact->m_p2pSession && contact->m_p2pSession->m_moniker == p_moniker)
            return contact->m_p2pSession;
    }

    return 0; // Session not found
}

void XfireAccount::slotOpenGamesManager()
{
    m_gamesManager->show();
}

void XfireAccount::slotVersionUpdated()
{
    Kopete::InfoEvent *event = new Kopete::InfoEvent();
    event->setTitle(i18n("Xfire version update"));
    event->setText(i18n("The Xfire version has been updated and you will be reconnected."));
    event->sendEvent();
}
