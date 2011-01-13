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
#include "xf_p2p_natcheck.h"
#include "xf_server.h"

#include <QCryptographicHash>

XfireContact::XfireContact(Kopete::Account *p_account, const QString &p_uniqueName, const QString &p_displayName, Kopete::MetaContact *p_parent) :
    Kopete::Contact(p_account, p_uniqueName, p_parent), m_username(p_uniqueName), m_chatSession(0L), m_chatMessageIndex(0),
    m_contactType(XF_FRIEND), m_p2pCapable(XF_P2P_UNKNOWN), m_p2pSession(NULL), m_p2pRequested(false)
{
    m_account = static_cast<XfireAccount *>(p_account);

    setOnlineStatus(XfireProtocol::protocol()->XfireOffline); // Set initial contact status to offline

    m_avatarManager = new QNetworkAccessManager(this);
    connect(m_avatarManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(slotGotAvatar(QNetworkReply *)));

    // Get and/or set the avatar
    QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + m_username + ".jpg"));
    QFileInfo file(imageLocation);

    if(file.exists())
        Kopete::Contact::setPhoto(imageLocation);
    else
        updateAvatar();

    removeProperties(); // Remove old properties
}

void XfireContact::slotGotAvatar(QNetworkReply *pReply)
{
    if(pReply->error() == QNetworkReply::ContentNotFoundError)
        return;

    QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + m_username + ".jpg"));
    QByteArray rawImage = pReply->readAll();

    if(rawImage.isEmpty())
        return;

    QImage image;
    image.loadFromData(rawImage, "JPG");
    image.save(imageLocation, "JPG", -1);

    Kopete::Contact::setPhoto(imageLocation);
    pReply->deleteLater();
}

XfireContact::~XfireContact()
{
}

void XfireContact::updateAvatar()
{
    updateAvatar(0);
}

void XfireContact::updateAvatar(quint32 p_number)
{
    m_avatarManager->get(QNetworkRequest(QUrl("http://screenshot.xfire.com/avatar/" + m_username + ".jpg?" + QString(p_number))));
}

void XfireContact::serialize(QMap<QString, QString> &serializedData, QMap<QString, QString> &addressBookData)
{
    Kopete::Contact::serialize(serializedData, addressBookData);
}

void XfireContact::slotChatSessionDestroyed()
{
    m_chatSession->deref();
    m_chatSession = 0;
}

Kopete::ChatSession* XfireContact::manager(CanCreateFlags p_canCreateFlags)
{
    if(m_chatSession != 0L || p_canCreateFlags == Kopete::Contact::CannotCreate)
        return m_chatSession;

    QList<Kopete::Contact *>contacts;
    contacts.append(this);

    m_chatSession = Kopete::ChatSessionManager::self()->create(account()->myself(), contacts, protocol());

    // Signals
    connect(m_chatSession, SIGNAL(messageSent(Kopete::Message &, Kopete::ChatSession *)), this, SLOT(sendMessage(Kopete::Message &)));
    connect(m_chatSession, SIGNAL(destroyed()), this, SLOT(slotChatSessionDestroyed()));
    connect(m_chatSession, SIGNAL(myselfTyping(bool)), this, SLOT(slotSendTyping(bool)));

    return m_chatSession;
}

void XfireContact::slotSendTyping(bool p_isTyping)
{
    if(isPeerToPeerActive())
        m_p2pSession->sendTypingStatus(m_chatMessageIndex, p_isTyping);
    else
        m_account->server()->sendTypingStatus(m_sid, m_chatMessageIndex, p_isTyping);
}

void XfireContact::sendMessage(Kopete::Message &p_message)
{
    if(isPeerToPeerActive()) // Send IM through P2P
    {
        kDebug() << m_username + ": sending p2p im";
        m_p2pSession->sendMessage( m_chatMessageIndex, p_message.plainBody());
    }
    else // Send IM through TCP
    {
        kDebug() << m_username + ": sending im";
        m_account->server()->sendChat(m_sid, m_chatMessageIndex, p_message.plainBody());
    }

    m_chatSession->appendMessage(p_message); // Append message
    m_chatMessageIndex++; // Raise chat message index

    // Request P2P information if not known yet
    if(m_p2pCapable == XF_P2P_UNKNOWN && m_account->isPeerToPeerEnabled())
        requestP2P();
}

void XfireContact::requestP2P()
{
    if(!m_p2pSession)
    {
        // Generate random salt
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        QString rndStr = QString::number(rand());

        hasher.addData(rndStr.toAscii());
        QString randomHash = hasher.result().toHex();

        createP2pSession(randomHash);
        m_account->server()->sendP2pSession(m_sid, m_account->m_p2pConnection->m_natCheck->m_ips[0], m_account->m_p2pConnection->m_connection->localPort(),
                                                m_account->server()->m_connection->localAddress().toIPv4Address(), m_account->m_p2pConnection->m_connection->localPort(),
                                                m_account->m_p2pConnection->m_natCheck->m_type, randomHash);
    }
}

void XfireContact::receivedMessage(const QString &p_message)
{
    Kopete::Message kmessage(this, account()->myself());
    kmessage.setPlainBody(p_message);
    kmessage.setDirection(Kopete::Message::Inbound);

    manager(Kopete::Contact::CanCreate)->appendMessage(kmessage);
}

void XfireContact::setOnlineStatus(const Kopete::OnlineStatus &status)
{
    if(status == XfireProtocol::protocol()->XfireOnline)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
    else if(status == XfireProtocol::protocol()->XfireAway)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireAway);
    else if(status == XfireProtocol::protocol()->XfireOffline)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOffline);
}

void XfireContact::setId(Xfire::Int32Attribute *p_id)
{
    m_id = p_id->value();
}

void XfireContact::setMessageSucceeded()
{
    manager(Kopete::Contact::CanCreate)->messageSucceeded();
}

void XfireContact::setTypingStatus(bool p_status)
{
    manager(Kopete::Contact::CanCreate)->receivedTypingMsg(this, p_status);
}

void XfireContact::removeProperties()
{
    removeProperty(XfireProtocol::protocol()->propGame);
    removeProperty(XfireProtocol::protocol()->propServer);
}

bool XfireContact::isPeerToPeerActive()
{
    if(m_p2pSession != NULL)
        return TRUE;
    else
        return FALSE;
}

void XfireContact::createP2pSession(const QString &p_salt)
{
    m_p2pSession = new XfireP2PSession(this, p_salt);
    QObject::connect(m_p2pSession, SIGNAL(timeout()), this, SLOT(slotRemoveP2pSession()));
}

void XfireContact::slotRemoveP2pSession()
{
    delete m_p2pSession;
    m_p2pSession = 0L;
}