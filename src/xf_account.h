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
class XfireGameDetection;
class XfireGamesManager;
class XfireP2P;
class XfireP2PSession;
class XfireProtocol;
class XfireServer;

class XfireAccount : public Kopete::PasswordedAccount
{
    Q_OBJECT

public:
    // Constructor and destructor
    XfireAccount(XfireProtocol *parent, const QString &accountID);
    ~XfireAccount();

    Xfire::SessionID m_sid;

    virtual bool createContact(const QString &p_contactId, Kopete::MetaContact *p_parentContact);
    virtual void fillActionMenu(KActionMenu *p_actionMenu);

    // Xfire server and port
    QString serverName() const;
    uint serverPort() const;
    uint protocolVersion() const;

    XfireServer *server();

    // Games list
    XfireGamesList *m_gamesList;
    XfireGameDetection *m_gamesDetection;

    void updateContactID(const QString &p_contactId, Xfire::Int32Attribute *p_id);
    void updateContactGameInformation(const Xfire::SessionID &p_sid, quint32 p_gameId);
    void setStatus(const Xfire::SessionID &p_sid, const QString &p_statusMessage);
    void newContact(const QString &p_contactId, const QString &p_name, int p_groupId);

    Kopete::Contact *findContact(QString p_contactId);
    Kopete::Contact *findContact(const Xfire::SessionID &p_sid);
    Kopete::Contact *findContact(quint32 p_id);

    bool isGameDetectionEnabled();
    bool isInformAccountsEnabled();
    bool isFriendsOfFriendsEnabled();
    bool isPeerToPeerEnabled();

    void logOff(Kopete::Account::DisconnectReason p_reason);

    // P2P
    XfireP2P *m_p2pConnection;
    XfireP2PSession *p2pSessionByMoniker(QByteArray p_moniker);

private:
    XfireServer *m_server;
    KAction *m_openGamesManager;
    XfireGamesManager *m_gamesManager;

protected:
    QString m_password;

public slots:
    virtual void connectWithPassword(const QString &p_password);
    virtual void disconnect();

    virtual void setOnlineStatus(const Kopete::OnlineStatus& p_status , const Kopete::StatusMessage &p_reason = Kopete::StatusMessage(), const OnlineStatusOptions &p_options = None);
    virtual void setStatusMessage(const Kopete::StatusMessage &p_statusMessage);

    void changeOurStatus(const Kopete::OnlineStatus &p_status);
    void updateContactSID(Xfire::Int32Attribute *p_id, Xfire::SIDAttribute *p_sid);

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
