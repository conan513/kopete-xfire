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
#include "kopeteaddedinfoevent.h"
#include <kopetecontact.h>
#include <kopetecontactlist.h>
#include <kopetegroup.h>
#include <kopetemetacontact.h>
#include <kopetepasswordedaccount.h>

#include "xf_account.h"
#include "xf_contact.h"
#include "xf_protocol.h"
#include "xf_server.h"
#include "xf_games_list.h"
#include "xf_game_detection.h"
#include "xf_games_manager.h"

XfireAccount::XfireAccount(XfireProtocol *parent, const QString &accountId) : Kopete::PasswordedAccount(parent, accountId)
{
	kDebug() << "Instantiating account " + accountId + ".";

	// Create the myself contact and set the initial status to offline
	setMyself(new XfireContact(this, accountId, accountId, Kopete::ContactList::self()->myself()));
	myself()->setOnlineStatus(parent->XfireOffline);

	mOpenGamesManager = new KAction(KIcon("input-gaming"), i18n("Configure games"), this);
	QObject::connect(mOpenGamesManager, SIGNAL(triggered()), this, SLOT(slotOpenGamesManager()));
}

XfireAccount::~XfireAccount()
{
	delete mGamesManager;
	delete mGamesDetection;
}

void XfireAccount::fillActionMenu(KActionMenu *pActionMenu)
{
	Kopete::Account::fillActionMenu(pActionMenu);

	mOpenGamesManager->setEnabled(true);
	pActionMenu->addSeparator();
	pActionMenu->addAction(mOpenGamesManager);
}

void XfireAccount::connectWithPassword(const QString &pPassword)
{
	// Cancel connection if no password has been supplied
	if(pPassword.isEmpty())
	{
		emit slotGoOffline();
		return;
	}

	// Don't attempt to connect when already online
	if(myself()->onlineStatus() != XfireProtocol::protocol()->XfireOffline)
		return;

	mPassword = pPassword;

	mGamesList = new XfireGamesList();
	QObject::connect(mGamesList, SIGNAL(gamesListReady()), this, SLOT(slotContinueConnecting()));
}

void XfireAccount::slotContinueConnecting()
{
	kDebug() << "Starting login sequence.";

	// Initialize server
	myself()->setOnlineStatus(XfireProtocol::protocol()->XfireConnecting); // Set Kopete status to connecting
	mServer = new XfireServer(this, accountId(), mPassword, serverName(), serverPort()); // Make new connection to the Xfire server

	mGamesManager = new XfireGamesManager(this); // Initialize games manager

	// Initialize game detection if enabled
	if(isGameDetectionEnabled())
	{
		mGamesDetection = new XfireGameDetection(this);
		QObject::connect(mGamesDetection, SIGNAL(gameRunning()), this, SLOT(slotSendIngameStatus()));
	}

	// Initialize P2P if enabled
	// mP2PConnection = new XfireP2P(this);
}

void XfireAccount::slotSendIngameStatus()
{
	mServer->sendIngameStatus(mGamesDetection->mCurrentGame.id, mGamesDetection->mCurrentGame.ip, mGamesDetection->mCurrentGame.port);

	// Inform other Kopete accounts (Xfire: Game)
	if(isInformAccountsEnabled())
	{
		QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts();
		foreach(Kopete::Account *a, accounts)
		{
			if(a->protocol() == myself()->protocol())
				continue;

			Kopete::StatusMessage message;
			if(mGamesDetection->mCurrentGame.id == 0)
				message.setMessage("");
			else
				message.setMessage("Xfire: " + mGamesList->getGameNameFromID(mGamesDetection->mCurrentGame.id));

			a->setStatusMessage(message);
		}
	}
}

void XfireAccount::logOff(Kopete::Account::DisconnectReason pReason)
{
	kDebug() << "Disconnecting from the Xfire server.";

	mServer->closeConnection();
	myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOffline);

	if(pReason == Kopete::Account::OtherClient)
		kDebug() << "Logged in somewhere else.";

	disconnected(pReason);
}

void XfireAccount::disconnect()
{
	logOff(Kopete::Account::Manual);
}

bool XfireAccount::createContact(const QString &pContactID, Kopete::MetaContact *pParentContact)
{
	kDebug() << "Creating contact" << pContactID << "(display name: " + pParentContact->displayName() + ").";
	XfireContact *newContact = new XfireContact(this, pContactID, pParentContact->displayName(), pParentContact);
	return newContact != 0;
}

void XfireAccount::setStatusMessage(const Kopete::StatusMessage &pStatusMessage)
{
	kDebug() << "Setting status message.";

	if(isConnected())
		mServer->sendStatusMessage(pStatusMessage.message());
}

void XfireAccount::slotGoOnline()
{
	if(!isConnected())
		connect(XfireProtocol::protocol()->XfireConnecting);
	else
		myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
}

void XfireAccount::slotGoAway()
{
	if(!isConnected())
		connect(XfireProtocol::protocol()->XfireConnecting);

	myself()->setOnlineStatus(XfireProtocol::protocol()->XfireAway);
	server()->sendStatusMessage("(AFK) Away From Keyboard");
}

void XfireAccount::slotGoOffline()
{
	if(isConnected() || myself()->onlineStatus().status() == Kopete::OnlineStatus::Connecting)
		disconnect();
}

void XfireAccount::newContact(const QString &pContactID, const QString &pName, int pGroupID)
{
	Kopete::Group *group;
	if(pGroupID != -1)
		group = Kopete::ContactList::self()->group(pGroupID);
	else
		group = Kopete::ContactList::self()->findGroup("Xfire");

	XfireContact *contact = static_cast<XfireContact *>(contacts().value(pContactID));
	if(!contact)
		addContact(pContactID, pName, group);
	else
		contact->metaContact()->addToGroup(group);
}

void XfireAccount::updateContactID(const QString &pContactID, Xfire::Int32Attribute *pID)
{
	XfireContact *c;
	c = static_cast<XfireContact*>(contacts().value(pContactID));

	if(c)
		c->setID(pID);
}

void XfireAccount::updateContactSID(Xfire::Int32Attribute *pID, Xfire::SIDAttribute *pSID)
{
	XfireContact *c = (XfireContact*)findContact(pID->value());
	if(c != 0)
	{
		if(c->mID == pID->value())
		{
			// Update the SID of the contact
			c->mSession = pSID->sid();

			// Set the online status of the contact
			if(pSID->sid().isValid())
				c->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
			else
				c->setOnlineStatus(XfireProtocol::protocol()->XfireOffline);
		}
	}
}

void XfireAccount::updateContactGameInformation(const Xfire::SessionID &pSID, quint32 pGameID)
{
	XfireContact *c;
	c = static_cast<XfireContact*>(findContact(pSID));
	
	if(c != 0)
	{
		QString message;
		QString gameName = mGamesList->getGameNameFromID(pGameID);

		if(pGameID == 0)
			message = "";
		else if(gameName == 0L)
			message = "Playing " + QString::number(pGameID);
		else
			message = "Playing " + gameName;

		Kopete::StatusMessage status(message);
		c->setStatusMessage(status);
	}
}

void
XfireAccount::setStatus(const Xfire::SessionID &pSID, const QString &pStatusMessage)
{
	XfireContact *c;
	c = static_cast<XfireContact*>(findContact(pSID));
	
	if(c != 0)
	{
		Kopete::StatusMessage status(pStatusMessage);
		c->setStatusMessage(status);

		if(pStatusMessage.contains("(AFK)"))
			c->setOnlineStatus(XfireProtocol::protocol()->XfireAway);
		else if(c->Away)
			c->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
	}
}

Kopete::Contact* XfireAccount::findContact(quint32 pID)
{
	QHash<QString, Kopete::Contact*> hash;
	hash = contacts();

	QHash<QString, Kopete::Contact*>::iterator i;

	for(i = hash.begin(); i != hash.end(); ++i)
	{
		XfireContact *contact = static_cast<XfireContact*>(i.value());
		if(contact->mID == pID)
			return i.value();
	}

	return 0; // Contact not found
}


Kopete::Contact* XfireAccount::findContact(const Xfire::SessionID &pSID)
{
	QHash<QString, Kopete::Contact*> hash;
	hash = contacts();

	QHash<QString, Kopete::Contact*>::iterator i;

	for(i = hash.begin(); i != hash.end(); ++i)
	{
		XfireContact *contact = static_cast<XfireContact*>(i.value());
		if(contact->mSession == pSID)
			return i.value();
	}

	return 0; // Contact not found
}

Kopete::Contact* XfireAccount::findContact(QString pContactId)
{
	QHash<QString, Kopete::Contact*> hash;
	hash = contacts();

	QHash<QString, Kopete::Contact*>::iterator i;

	for(i = hash.begin(); i != hash.end(); ++i)
	{
		XfireContact *contact = static_cast<XfireContact*>(i.value());
		if(contact->contactId() == pContactId)
			return i.value();
	}

	return 0; // Contact not found
}

QString XfireAccount::serverName() const
{
	if(configGroup()->readEntry("CustomServer", false))
		return configGroup()->readEntry("ServerIP", "cs.xfire.com");
	else
		return "cs.xfire.com";
}

uint XfireAccount::serverPort() const
{

	if(configGroup()->readEntry("CustomServer", false))
		return configGroup()->readEntry("ServerPort", 25999);
	else
		return 25999;
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

void XfireAccount::setOnlineStatus(const Kopete::OnlineStatus &pStatus, const Kopete::StatusMessage &pReason, const OnlineStatusOptions &pOptions)
{
	if(isConnected())
	{
		kDebug() << "Setting status text to:" << pReason.message();
		mServer->sendStatusMessage(pReason.message());
	}

	kDebug() << "Setting online status to:" << pStatus.description();

	if(pStatus == XfireProtocol::protocol()->XfireConnecting && myself()->onlineStatus() == XfireProtocol::protocol()->XfireOffline)
		slotGoOnline();
	else if(pStatus == XfireProtocol::protocol()->XfireOnline || pStatus.status() == Kopete::OnlineStatus::Online)
		slotGoOnline();
	else if (pStatus == XfireProtocol::protocol()->XfireOffline)
		slotGoOffline();
	else if(pStatus.status() == Kopete::OnlineStatus::Away)
		slotGoAway();
}

void XfireAccount::changeOurStatus(const Kopete::OnlineStatus &pStatus)
{
	kDebug() << "Changing status to:" << pStatus.description();

	if(pStatus == XfireProtocol::protocol()->XfireOnline)
		myself()->setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
}

XfireServer* XfireAccount::server()
{
	return mServer;
}



XfireP2PSession* XfireAccount::p2pSessionByMoniker(QByteArray pMoniker)
{
	for(int i = 0; i < mP2PConnection->mSessions.size(); i++)
	{
		if(mP2PConnection->mSessions.at(i)->monikerSelf == pMoniker)
			return mP2PConnection->mSessions.at(i);
	}

	return 0; // Not found
}

void XfireAccount::slotOpenGamesManager()
{
	mGamesManager->show();
}

void XfireAccount::slotVersionUpdated()
{
	Kopete::InfoEvent *event = new Kopete::InfoEvent();
	event->setTitle("Xfire version update");
	event->setText("The Xfire version has been updated and you will be reconnected.");
	event->sendEvent();
}
