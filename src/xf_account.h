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

#ifndef XF_ACCOUNT_H
#define XF_ACCOUNT_H

#include <kopeteaccount.h>
#include <kopetepasswordedaccount.h>

#include "XfireAttribute.h"
#include "XfirePacket.h"
#include "XfireTypes.h"
#include "xf_games_list.h"

class KAction;
class KActionMenu;

class XfireProtocol;
class XfireServer;
class XfireGamesManager;
class XfireGameDetection;
class XfireP2P;
class XfireP2PSession;

class XfireAccount : public Kopete::PasswordedAccount
{
	Q_OBJECT

public:
	// Constructor and destructor
	XfireAccount(XfireProtocol *parent, const QString &accountID);
	~XfireAccount();

	Xfire::SessionID mSID;

	virtual bool createContact(const QString &pContactId, Kopete::MetaContact *pParentContact);
	virtual void fillActionMenu(KActionMenu *pActionMenu);

	// Xfire server and port
	QString serverName() const;
	uint serverPort() const;
	uint protocolVersion() const;

	XfireServer *server();

	// Games list
	XfireGamesList *mGamesList;
	XfireGameDetection *mGamesDetection;

	void updateContactID(const QString &pContactId, Xfire::Int32Attribute *pID);
	void updateContactGameInformation(const Xfire::SessionID &pSID, quint32 pGameID);
	void setStatus(const Xfire::SessionID &pSID, const QString &pStatusMessage);
	void newContact(const QString &pContactId, const QString &pName, int pGroupID);

	Kopete::Contact *findContact(QString pContactId);
	Kopete::Contact *findContact(const Xfire::SessionID &pSID);
	Kopete::Contact *findContact(quint32 pID);

	bool isGameDetectionEnabled();
	bool isInformAccountsEnabled();
	bool isFriendsOfFriendsEnabled();

	void logOff(Kopete::Account::DisconnectReason pReason);

	// P2P
	XfireP2P *mP2PConnection;

	XfireP2PSession *p2pSessionByMoniker(QByteArray pMoniker);

private:
	XfireServer *mServer;
	KAction *mOpenGamesManager;
	XfireGamesManager *mGamesManager;

protected:
	QString mPassword;

public slots:
	virtual void connectWithPassword(const QString &pPassword);
	virtual void disconnect();

	virtual void setOnlineStatus(const Kopete::OnlineStatus& pStatus , const Kopete::StatusMessage &pReason = Kopete::StatusMessage(), const OnlineStatusOptions &pOptions = None);
	virtual void setStatusMessage(const Kopete::StatusMessage &pStatusMessage);

	void changeOurStatus(const Kopete::OnlineStatus &pStatus);
	void updateContactSID(Xfire::Int32Attribute *pID, Xfire::SIDAttribute *pSID);

	void slotOpenGamesManager();

	void slotGoOffline();
	void slotGoOnline();
	void slotGoAway();

	void slotVersionUpdated();

protected slots:
	void slotContinueConnecting();
	void slotSendIngameStatus();
};

#endif // XF_ACCOUNT_H
