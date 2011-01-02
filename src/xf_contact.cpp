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

XfireContact::XfireContact(Kopete::Account *pAccount, const QString &uniqueName, const QString &displayName, Kopete::MetaContact *parent) : Kopete::Contact(pAccount, uniqueName, parent)
{
    m_account = static_cast<XfireAccount *>(pAccount);

    m_username = uniqueName;

    m_chatSession = 0L; // Initialize message manager and account
    m_chatMessageIndex = 0; // Initialize chat message index
    setOnlineStatus(XfireProtocol::protocol()->XfireOffline); // Set initial contact status to offline

    m_contactType = XF_FRIEND;
    m_p2pCapable = XF_P2P_UNKNOWN;
    m_p2pSession = NULL;
    m_p2pRequested = FALSE;

    // Get and/or set the avatar
    QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + m_username + ".jpg"));
    QFileInfo file(imageLocation);

    if (file.exists())
        Kopete::Contact::setPhoto(imageLocation);
    else
        updateAvatar();
}

void XfireContact::slotGotAvatar(QNetworkReply *pReply)
{
    if (pReply->error() == QNetworkReply::ContentNotFoundError)
        return;

    QString imageLocation(KStandardDirs::locateLocal("appdata", "xfire/avatars/" + m_username + ".jpg"));
    QByteArray rawImage = pReply->readAll();

    if (rawImage.isEmpty())
        return;

    QImage image;
    image.loadFromData(rawImage, "JPG");
    image.save(imageLocation, "JPG", -1);

    Kopete::Contact::setPhoto(imageLocation);
}

XfireContact::~XfireContact()
{
}

void XfireContact::updateAvatar()
{
    updateAvatar(0);
}

void XfireContact::updateAvatar(quint32 pNumber)
{
    QNetworkAccessManager *avatarManager;
    avatarManager = new QNetworkAccessManager(this);
    connect(avatarManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(slotGotAvatar(QNetworkReply *)));
    avatarManager->get(QNetworkRequest(QUrl("http://screenshot.xfire.com/avatar/" + m_username + ".jpg?" + QString(pNumber))));
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

Kopete::ChatSession* XfireContact::manager(CanCreateFlags canCreateFlags)
{
    if (m_chatSession != 0L || canCreateFlags == Kopete::Contact::CannotCreate)
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
    XfireAccount *acc = static_cast<XfireAccount *>(account());
    acc->server()->sendTypingStatus(m_session, m_chatMessageIndex, p_isTyping);
}

void XfireContact::sendMessage(Kopete::Message &p_message)
{
    // Request P2P information if not known yet
    if (m_p2pCapable == XF_P2P_UNKNOWN)
        requestP2P();

    XfireAccount *acc = static_cast<XfireAccount *>(account());

    // Send IM through P2P
    if (m_p2pCapable == XF_P2P_YES)
    {
        kDebug() << "Sending P2P IM to:" << m_username;
        // FIXME: Not implemented yet
    }
    else // Send IM through TCP
        acc->server()->sendChat(m_session, m_chatMessageIndex, p_message.plainBody());

    m_chatSession->appendMessage(p_message); // Append it
    m_chatMessageIndex++; // Update chat message index
}

void XfireContact::requestP2P()
{
    if (!m_p2pSession)
    {
        // Generate random salt
        /*QCryptographicHash hasher(QCryptographicHash::Sha1);

        int rnd = rand();
        QString rndStr = QString::number(rnd);

        kDebug() << rndStr;

        hasher.addData(rndStr.toAscii());
        QString randomHash = hasher.result().toHex();

        if(!m_p2pSession)
        {
            kDebug() << "Creating session";
            m_p2pSession = new XfireP2PSession(this, randomHash);
            m_account->m_p2pConnection->addSession(m_p2pSession);
        }
        */

        /*m_account->server()->sendP2pSession(m_session, m_account->m_p2pConnection->m_natCheck->m_ips[0], m_account->m_p2pConnection->m_connection->localPort(),
                                            m_account->server()->m_connection->localAddress().toIPv4Address(), m_account->m_p2pConnection->m_connection->localPort(),
                                            m_account->m_p2pConnection->m_natCheck->m_type, m_account->m_);*/

        /*m_p2pRequested = TRUE;*/
    }
}

void XfireContact::receivedMessage(const QString &pMessage)
{
    Kopete::Message kmessage(this, account()->myself());
    kmessage.setPlainBody(pMessage);
    kmessage.setDirection(Kopete::Message::Inbound);

    manager(Kopete::Contact::CanCreate)->appendMessage(kmessage);
}

void XfireContact::setOnlineStatus(const Kopete::OnlineStatus &status)
{
    if (status == XfireProtocol::protocol()->XfireOnline)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOnline);
    else if (status == XfireProtocol::protocol()->XfireAway)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireAway);
    else if (status == XfireProtocol::protocol()->XfireOffline)
        Kopete::Contact::setOnlineStatus(XfireProtocol::protocol()->XfireOffline);
}

void XfireContact::setId(Xfire::Int32Attribute *p_id)
{
    m_id = p_id->value();

    // FIXME: This should be asked only on request, but not possible yet (get info not implemented in UI)
    XfireAccount *acc = static_cast<XfireAccount *>(account());
    acc->server()->sendAskFriendExtendedInformation(m_id);
}

void XfireContact::setMessageSucceeded()
{
    manager(Kopete::Contact::CanCreate)->messageSucceeded();
}

void XfireContact::setTypingStatus(bool pStatus)
{
    manager(Kopete::Contact::CanCreate)->receivedTypingMsg(this, pStatus);
}
