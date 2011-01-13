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

#ifndef XF_CONTACT_H
#define XF_CONTACT_H

#include <QtNetwork/QNetworkReply>

#include <kopetecontact.h>
#include <kopetemessage.h>
#include <kopetemetacontact.h>

#include "XfireAttribute.h"
#include "XfirePacket.h"
#include "XfireTypes.h"

#include "xf_account.h"
#include "xf_p2p.h"
#include "xf_p2p_session.h"

class XfireContact : public Kopete::Contact
{
    Q_OBJECT

public:
    typedef enum
    {
        XF_FRIEND = 0,
        XF_CLAN, XF_GROUPCHAT,
        XF_FRIEND_OF_FRIEND
    } ContactType;

    typedef enum
    {
        XF_P2P_UNKNOWN,
        XF_P2P_YES,
        XF_P2P_NO
    } CanHandleP2P;

    // Constructor and destructor
    XfireContact(Kopete::Account *pAccount, const QString &uniqueName, const QString &displayName, Kopete::MetaContact *parent);
    ~XfireContact();

    XfireAccount *m_account;

    void receivedMessage(const QString &p_message); // FIXME: Use slot instead

    virtual void serialize(QMap<QString, QString> &serializedData, QMap<QString, QString> &);
    void setId(Xfire::Int32Attribute *p_id);
    void setMessageSucceeded();
    void setOnlineStatus(const Kopete::OnlineStatus &p_status);
    void setTypingStatus(bool p_status);

    // Xfire user data
    quint32 m_id;
    Xfire::SessionID m_sid;
    QString m_username;
    void removeProperties();

    // P2P
    CanHandleP2P m_p2pCapable;
    bool m_p2pRequested;
    XfireP2PSession *m_p2pSession;
    void requestP2P();
    bool isPeerToPeerActive();
    void createP2pSession(const QString& p_salt);

    // Chatting
    virtual Kopete::ChatSession *manager(CanCreateFlags p_canCreateFlags = CannotCreate); // Returns the Kopete chat session associated with the contact
    quint32 m_chatMessageIndex;

    // Contact type
    ContactType m_contactType;

protected:
    Kopete::ChatSession *m_chatSession;
    QNetworkAccessManager *m_avatarManager;

public slots:
    void updateAvatar();
    void updateAvatar(quint32 p_number);
    void sendMessage(Kopete::Message &p_message);
    void slotChatSessionDestroyed();
    void slotSendTyping(bool p_isTyping);

private slots:
    void slotGotAvatar(QNetworkReply *p_reply);
    void slotRemoveP2pSession();
};

#endif // XF_CONTACT_H
