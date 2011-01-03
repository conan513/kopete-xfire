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
#include "xf_games_manager.h"
#include "xf_games_list.h"
#include "xf_protocol.h"
#include "xf_server.h"

XfireAccount::XfireAccount ( XfireProtocol *parent, const QString &accountId ) : Kopete::PasswordedAccount ( parent, accountId ), m_gamesDetection ( 0 ), m_gamesManager ( 0 )
{
    kDebug() << "Instantiating account:" << accountId;

    // Create the myself contact and set the initial status to offline
    setMyself ( new XfireContact ( this, accountId, accountId, Kopete::ContactList::self()->myself() ) );
    myself()->setOnlineStatus ( parent->XfireOffline );

    m_gamesList = new XfireGamesList();
    m_gamesManager = new XfireGamesManager ( this );
    m_openGamesManager = new KAction ( KIcon ( "input-gaming" ), i18n ( "Configure games" ), this );

    QObject::connect ( m_gamesList, SIGNAL ( gamesListReady() ), this, SLOT ( slotContinueConnecting() ) );
    QObject::connect ( m_gamesList, SIGNAL ( gamesListReady() ), m_gamesManager, SLOT ( slotUpdate() ) );
    QObject::connect ( m_openGamesManager, SIGNAL ( triggered() ), this, SLOT ( slotOpenGamesManager() ) );
}

XfireAccount::~XfireAccount()
{
    delete m_gamesManager;

    if ( m_gamesDetection )
        delete m_gamesDetection;
}

void XfireAccount::fillActionMenu ( KActionMenu *p_actionMenu )
{
    Kopete::Account::fillActionMenu ( p_actionMenu );

    m_openGamesManager->setEnabled ( true );
    p_actionMenu->addSeparator();
    p_actionMenu->addAction ( m_openGamesManager );
}

void XfireAccount::connectWithPassword ( const QString &p_password )
{
    // Cancel connection if no password has been supplied
    if ( p_password.isEmpty() )
    {
        emit slotGoOffline();
        return;
    }

    // Don't attempt to connect when already online
    if ( myself()->onlineStatus() != XfireProtocol::protocol()->XfireOffline )
        return;

    m_password = p_password;

    m_gamesList->slotUpdate();
}

void XfireAccount::slotContinueConnecting()
{
    kDebug() << "Starting login sequence";

    // Initialize server
    myself()->setOnlineStatus ( XfireProtocol::protocol()->XfireConnecting ); // Set Kopete status to connecting
    m_server = new XfireServer ( this, accountId(), m_password, serverName(), serverPort() ); // Make new connection to the Xfire server

    // Initialize game detection if enabled
    if ( isGameDetectionEnabled() )
    {
        m_gamesDetection = new XfireGameDetection ( this );
        QObject::connect ( m_gamesDetection, SIGNAL ( gameRunning() ), this, SLOT ( slotSendIngameStatus() ) );
    }

    // Initialize peer to peer if enabled
    if ( isPeerToPeerEnabled() )
        m_p2pConnection = new XfireP2P ( this );
}

void XfireAccount::slotSendIngameStatus()
{
    m_server->sendIngameStatus ( m_gamesDetection->m_currentGame.id, m_gamesDetection->m_currentGame.ip, m_gamesDetection->m_currentGame.port );

    // Inform other Kopete accounts (Xfire: Game)
    if ( isInformAccountsEnabled() )
    {
        QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts();
        foreach ( Kopete::Account *a, accounts )
        {
            if ( a->protocol() == myself()->protocol() )
                continue;

            Kopete::StatusMessage message;
            if ( m_gamesDetection->m_currentGame.id == 0 )
                message.setMessage ( "" );
            else
                message.setMessage ( "Xfire: " + m_gamesList->getGameNameFromID ( m_gamesDetection->m_currentGame.id ) );

            a->setStatusMessage ( message );
        }
    }
}

void XfireAccount::logOff ( Kopete::Account::DisconnectReason p_reason )
{
    kDebug() << "Disconnecting from the Xfire server";

    m_server->closeConnection();
    myself()->setOnlineStatus ( XfireProtocol::protocol()->XfireOffline );

    if ( p_reason == Kopete::Account::OtherClient )
        kDebug() << "Logged in somewhere else";

    disconnected ( p_reason );
}

void XfireAccount::disconnect()
{
    logOff ( Kopete::Account::Manual );
}

bool XfireAccount::createContact ( const QString &p_contactId, Kopete::MetaContact *p_parentContact )
{
    kDebug() << "Creating contact:" << p_contactId;
    XfireContact *newContact = new XfireContact ( this, p_contactId, p_parentContact->displayName(), p_parentContact );
    return newContact != 0;
}

void XfireAccount::setStatusMessage ( const Kopete::StatusMessage &p_statusMessage )
{
    if ( isConnected() )
    {
        kDebug() << "Setting status message:" + p_statusMessage.message();
        m_server->sendStatusMessage ( p_statusMessage.message() );
    }
}

void XfireAccount::slotGoOnline()
{
    if ( !isConnected() )
        connect ( XfireProtocol::protocol()->XfireConnecting );
    else
        myself()->setOnlineStatus ( XfireProtocol::protocol()->XfireOnline );
}

void XfireAccount::slotGoAway()
{
    if ( !isConnected() )
        connect ( XfireProtocol::protocol()->XfireConnecting );

    myself()->setOnlineStatus ( XfireProtocol::protocol()->XfireAway );
    server()->sendStatusMessage ( "(AFK) Away From Keyboard" );
}

void XfireAccount::slotGoOffline()
{
    if ( isConnected() || myself()->onlineStatus().status() == Kopete::OnlineStatus::Connecting )
        disconnect();
}

void XfireAccount::newContact ( const QString &p_contactId, const QString &p_name, int p_groupId )
{
    Kopete::Group *group;
    if ( p_groupId != -1 )
        group = Kopete::ContactList::self()->group ( p_groupId );
    else
        group = Kopete::ContactList::self()->findGroup ( "Xfire" );

    XfireContact *contact = static_cast<XfireContact *> ( contacts().value ( p_contactId ) );
    if ( !contact )
        addContact ( p_contactId, p_name, group );
    else
        contact->metaContact()->addToGroup ( group );
}

void XfireAccount::updateContactID ( const QString &p_contactId, Xfire::Int32Attribute *p_id )
{
    XfireContact *c = static_cast<XfireContact*> ( contacts().value ( p_contactId ) );
    if ( c )
        c->setId ( p_id );
}

void XfireAccount::updateContactSID ( Xfire::Int32Attribute *p_id, Xfire::SIDAttribute *p_sid )
{
    XfireContact *c = ( XfireContact* ) findContact ( p_id->value() );
    if ( c != 0 )
    {
        if ( c->m_id == p_id->value() )
        {
            c->m_session = p_sid->sid(); // Update the SID of the contact

            // Set the online status of the contact
            if ( p_sid->sid().isValid() )
                c->setOnlineStatus ( XfireProtocol::protocol()->XfireOnline );
            else
                c->setOnlineStatus ( XfireProtocol::protocol()->XfireOffline );
        }
    }
}

void XfireAccount::updateContactGameInformation ( const Xfire::SessionID &p_sid, quint32 p_gameId )
{
    XfireContact *c = static_cast<XfireContact*> ( findContact ( p_sid ) );
    if ( c != 0 )
    {
        QString message;
        QString gameName = m_gamesList->getGameNameFromID ( p_gameId );

        if ( p_gameId == 0 )
            message = "";
        else if ( gameName == 0L )
            message = "Playing " + QString::number ( p_gameId );
        else
            message = "Playing " + gameName;

        if ( gameName != 0L )
            c->setProperty ( XfireProtocol::protocol()->propGame, gameName );

        Kopete::StatusMessage status ( message );
        c->setStatusMessage ( status );
    }
}

void XfireAccount::setStatus ( const Xfire::SessionID &p_sid, const QString &p_statusMessage )
{
    XfireContact *c = static_cast<XfireContact*> ( findContact ( p_sid ) );
    if ( c != 0 )
    {
        Kopete::StatusMessage status ( p_statusMessage );
        c->setStatusMessage ( status );

        if ( p_statusMessage.contains ( "(AFK)" ) )
            c->setOnlineStatus ( XfireProtocol::protocol()->XfireAway );
        else if ( c->Away )
            c->setOnlineStatus ( XfireProtocol::protocol()->XfireOnline );
    }
}

Kopete::Contact* XfireAccount::findContact ( quint32 p_id )
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for ( i = hash.begin(); i != hash.end(); ++i )
    {
        XfireContact *contact = static_cast<XfireContact*> ( i.value() );
        if ( contact->m_id == p_id )
            return i.value();
    }

    return 0; // Contact not found
}


Kopete::Contact* XfireAccount::findContact ( const Xfire::SessionID &p_sid )
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for ( i = hash.begin(); i != hash.end(); ++i )
    {
        XfireContact *contact = static_cast<XfireContact*> ( i.value() );
        if ( contact->m_session == p_sid )
            return i.value();
    }

    return 0; // Contact not found
}

Kopete::Contact* XfireAccount::findContact ( QString p_contactId )
{
    QHash<QString, Kopete::Contact*> hash;
    hash = contacts();

    QHash<QString, Kopete::Contact*>::iterator i;
    for ( i = hash.begin(); i != hash.end(); ++i )
    {
        XfireContact *contact = static_cast<XfireContact*> ( i.value() );
        if ( contact->contactId() == p_contactId )
            return i.value();
    }

    return 0; // Contact not found
}

QString XfireAccount::serverName() const
{
    if ( configGroup()->readEntry ( "Custom_server", false ) )
        return configGroup()->readEntry ( "ServerIP", "cs.xfire.com" );
    else
        return "cs.xfire.com";
}

uint XfireAccount::serverPort() const
{

    if ( configGroup()->readEntry ( "Custom_server", false ) )
        return configGroup()->readEntry ( "ServerPort", 25999 );
    else
        return 25999;
}

uint XfireAccount::protocolVersion() const
{
    if ( configGroup()->readEntry ( "ProtocolVersion", false ) )
        return configGroup()->readEntry ( "ProtocolVersion", XFIRE_PROTO_VERSION );
    else
        return XFIRE_PROTO_VERSION;
}

bool XfireAccount::isGameDetectionEnabled()
{
    return configGroup()->readEntry ( "GameDetection", true );
}

bool XfireAccount::isInformAccountsEnabled()
{
    return configGroup()->readEntry ( "InformAccounts", false );
}

bool XfireAccount::isFriendsOfFriendsEnabled()
{
    return configGroup()->readEntry ( "FriendsOfFriends", true );
}

bool XfireAccount::isPeerToPeerEnabled()
{
    return configGroup()->readEntry ( "PeerToPeer", false );
}

void XfireAccount::setOnlineStatus ( const Kopete::OnlineStatus &p_status, const Kopete::StatusMessage &p_reason, const OnlineStatusOptions &p_options )
{
    Q_UNUSED ( p_options );

    if ( isConnected() )
    {
        kDebug() << "Setting status message to:" << p_reason.message();
        m_server->sendStatusMessage ( p_reason.message() );
    }

    kDebug() << "Setting online status to:" << p_status.description();

    if ( p_status == XfireProtocol::protocol()->XfireConnecting && myself()->onlineStatus() == XfireProtocol::protocol()->XfireOffline )
        slotGoOnline();
    else if ( p_status == XfireProtocol::protocol()->XfireOnline || p_status.status() == Kopete::OnlineStatus::Online )
        slotGoOnline();
    else if ( p_status == XfireProtocol::protocol()->XfireOffline )
        slotGoOffline();
    else if ( p_status.status() == Kopete::OnlineStatus::Away )
        slotGoAway();
}

void XfireAccount::changeOurStatus ( const Kopete::OnlineStatus &p_status )
{
    kDebug() << "Changing status to:" << p_status.description();

    if ( p_status == XfireProtocol::protocol()->XfireOnline )
        myself()->setOnlineStatus ( XfireProtocol::protocol()->XfireOnline );
}

XfireServer* XfireAccount::server()
{
    return m_server;
}

XfireP2PSession* XfireAccount::p2pSessionByMoniker ( QByteArray p_moniker )
{
    for ( int i = 0; i < m_p2pConnection->m_sessions.size(); i++ )
    {
        if ( m_p2pConnection->m_sessions.at ( i )->m_moniker == p_moniker )
            return m_p2pConnection->m_sessions.at ( i );
    }

    return 0;
}

void XfireAccount::slotOpenGamesManager()
{
    m_gamesManager->show();
}

void XfireAccount::slotVersionUpdated()
{
    Kopete::InfoEvent *event = new Kopete::InfoEvent();
    event->setTitle ( "Xfire version update" );
    event->setText ( "The Xfire version has been updated and you will be reconnected." );
    event->sendEvent();
}
