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

#include <QBuffer>
#include <QFile>
#include <QtNetwork>
#include <QtGui/QImage>

#include <KAction>
#include <KDebug>
#include <KLocale>
#include <KStandardDirs>

#include <kopeteaccount.h>
#include <kopetechatsessionmanager.h>
#include <kopetemetacontact.h>
#include <kopeteproperty.h>

#include "XfireAttribute.h"
#include "XfirePacket.h"
#include "XfireTypes.h"

#include "xf_account.h"
#include "xf_contact.h"
#include "xf_protocol.h"
#include "xf_server.h"

#include <QCryptographicHash>

XfireContact::XfireContact(Kopete::Account *pAccount, const QString &uniqueName, const QString &displayName, Kopete::MetaContact *parent)
	: Kopete::Contact(pAccount, uniqueName, parent)
{
	mAccount = static_cast<XfireAccount *>(pAccount);

	mUsername = uniqueName;

	mChatSession = 0L; // Initialize message manager and account
	mChatMessageIndex = 0; // Initialize chat message index
	setOnlineStatus(XfireProtocol::protocol()->XfireOffline); // Set initial contact status to offline

	// P2P
	P2PCapable = XF_P2P_UNKNOWN;

	// Buddy type
	contactType = XF_FRIEND;

	// Get and/or set the avatar
	QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + mUsername + ".jpg"));
	QFileInfo file(imageLocation);

	if(file.exists())
		Kopete::Contact::setPhoto(imageLocation);
	else
		updateAvatar();

	session = NULL;
}

void
XfireContact::slotGotAvatar(QNetworkReply *pReply)
{
	if(pReply->error() == QNetworkReply::ContentNotFoundError)
		return;

	QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + mUsername + ".jpg"));
	QByteArray rawImage = pReply->readAll();

	if(rawImage.isEmpty())
		return;

	QImage image;
	image.loadFromData(rawImage, "JPG");
	image.save(imageLocation, "JPG", -1);

	Kopete::Contact::setPhoto(imageLocation);
}

XfireContact::~XfireContact()
{
}

void
XfireContact::updateAvatar()
{
	updateAvatar(0);
}

void
XfireContact::updateAvatar(quint32 pNumber)
{
	QNetworkAccessManager *avatarManager;
	avatarManager = new QNetworkAccessManager(this);
	connect(avatarManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(slotGotAvatar(QNetworkReply *)));
	avatarManager->get(QNetworkRequest(QUrl("http://screenshot.xfire.com/avatar/" + mUsername + ".jpg?" + QString(pNumber))));
}

void
XfireContact::serialize(QMap<QString, QString> &serializedData, QMap<QString, QString> &addressBookData)
{
	Kopete::Contact::serialize(serializedData, addressBookData);
}

void
XfireContact::slotChatSessionDestroyed()
{
	mChatSession->deref();
	mChatSession = 0;
}

Kopete::ChatSession*
XfireContact::manager(CanCreateFlags canCreateFlags)
{
	if(mChatSession != 0L || canCreateFlags == Kopete::Contact::CannotCreate)
		return mChatSession;

	QList<Kopete::Contact *>contacts;
	contacts.append(this);

	mChatSession = Kopete::ChatSessionManager::self()->create(account()->myself(), contacts, protocol());

	// Signals
	connect(mChatSession, SIGNAL(messageSent(Kopete::Message &, Kopete::ChatSession *)), this, SLOT(sendMessage(Kopete::Message &)));
	connect(mChatSession, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
	connect(mChatSession, SIGNAL(myselfTyping(bool)), this, SLOT(slotSendTyping(bool)));

	return mChatSession;
}

void
XfireContact::slotSendTyping(bool pIsTyping)
{
	XfireAccount *acc = static_cast<XfireAccount *>(account());
	acc->server()->sendTypingStatus(mSession, mChatMessageIndex, pIsTyping);
}

void
XfireContact::sendMessage(Kopete::Message &message)
{
	// Ask P2P request if not known yet
	if(P2PCapable == XF_P2P_UNKNOWN)
	{
		// requestP2P();
	}

	XfireAccount *acc = static_cast<XfireAccount *>(account());

	// Send IM through P2P
	if(P2PCapable == XF_P2P_YES)
	{
		kDebug() << "Sending P2P IM to:" << mUsername;
		// FIXME: Not implemented yet
	}
	else // Send IM through TCP
		acc->server()->sendChat(mSession, mChatMessageIndex, message.plainBody());

	mChatSession->appendMessage(message); // Append it
	mChatMessageIndex++; // Update chat message index
}

void
XfireContact::requestP2P()
{
	if(!session)
	{
		// Generate random salt
		QCryptographicHash hasher(QCryptographicHash::Sha1);

		int rnd = rand();
		QString rndStr = QString::number(rnd);

		kDebug() << rndStr;

		hasher.addData(rndStr.toAscii());
		QString randomHash = hasher.result().toHex();

		kDebug() << "Creating session.";
		session = new XfireP2PSession(this, randomHash);
		mAccount->mP2PConnection->addSession(session);

		QString localIP = mAccount->server()->mConnection->localAddress().toString();
		QString remoteIP = "";

		mAccount->server()->sendP2pSession(mSession, QHostAddress(remoteIP).toIPv4Address(), mAccount->mP2PConnection->mConnection->localPort(),
										   QHostAddress(localIP).toIPv4Address(), mAccount->mP2PConnection->mConnection->localPort(), 4, randomHash);

		kDebug() << "Sent P2P request to:" << mUsername << randomHash;
		P2PRequested = TRUE;
	}
}

void
XfireContact::receivedMessage(const QString &pMessage)
{
	Kopete::Message kmessage(this, account()->myself());
	kmessage.setPlainBody(pMessage);
	kmessage.setDirection(Kopete::Message::Inbound);

	manager(Kopete::Contact::CanCreate)->appendMessage(kmessage);
}

void
XfireContact::setOnlineStatus(const Kopete::OnlineStatus &status)
{
	if(status == XfireProtocol::protocol()->XfireOnline)
		Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
	else if(status == XfireProtocol::protocol()->XfireAway)
		Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireAway);
	else if(status == XfireProtocol::protocol()->XfireOffline)
		Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOffline);
}

void
XfireContact::setID(Xfire::Int32Attribute *id)
{
	mID = id->value();

	// FIXME: This should be asked only on request, but not possible yet (get info not implemented in UI)
	XfireAccount *acc = static_cast<XfireAccount *>(account());
	acc->server()->sendAskFriendExtendedInformation(mID);
}

void
XfireContact::setMessageSucceeded()
{
	manager(Kopete::Contact::CanCreate)->messageSucceeded();
}

void
XfireContact::setTypingStatus(bool pStatus)
{
	manager(Kopete::Contact::CanCreate)->receivedTypingMsg(this, pStatus);
}
