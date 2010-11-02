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

#include <kopetecontact.h>
#include <kopetemessage.h>
#include <kopetemetacontact.h>
#include <QtNetwork/QNetworkReply>

#include "XfireAttribute.h"
#include "XfirePacket.h"
#include "XfireTypes.h"

#include "xf_account.h"
#include "xf_p2p.h"
#include "xf_p2p_session.h"

class XfireContact : public Kopete::Contact
{
	Q_OBJECT

	typedef enum {
		XF_FRIEND = 0,
		XF_CLAN, XF_GROUPCHAT,
		XF_FRIEND_OF_FRIEND
	} ContactType;

	typedef enum {
		XF_P2P_UNKNOWN,
		XF_P2P_YES,
		XF_P2P_NO
	} CanHandleP2P;

public:
	// Constructor and destructor
	XfireContact(Kopete::Account *pAccount, const QString &uniqueName, const QString &displayName, Kopete::MetaContact *parent);
	~XfireContact();

	void receivedMessage(const QString &pMessage); // FIXME: Use slot instead stupid!
	
	virtual void serialize(QMap<QString, QString> &serializedData, QMap<QString, QString> &);
	void setID(Xfire::Int32Attribute *id);
	void setMessageSucceeded();
	void setOnlineStatus(const Kopete::OnlineStatus& status);
	void setTypingStatus(bool pStatus);
	
	// Xfire user data
	quint32 mID;
	Xfire::SessionID mSession;
	QString mUsername;

	// P2P
	CanHandleP2P P2PCapable;
	bool P2PRequested;
	XfireP2PSession *session;
	void requestP2P();

	// Chatting
	virtual Kopete::ChatSession *manager(CanCreateFlags canCreate = CannotCreate); // Returns the Kopete chat session associated with the contact
	quint32 mChatMessageIndex;

	// Contact type
	ContactType contactType;

	XfireAccount *mAccount;

protected:
	Kopete::ChatSession *mChatSession;
	
public slots:
	void updateAvatar();
	void updateAvatar(quint32 pNumber);
	void sendMessage(Kopete::Message &message);
	void slotChatSessionDestroyed();
	void slotSendTyping(bool pIsTyping);

private slots:
	void slotGotAvatar(QNetworkReply *pReply);

};

#endif // XF_CONTACT_H
