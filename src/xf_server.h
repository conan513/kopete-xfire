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

#ifndef XF_SERVER_H
#define XF_SERVER_H

#include <QCryptographicHash>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QTimer>

#include "XfireAttribute.h"
#include "XfirePacket.h"
#include "XfireTypes.h"

#include "xf_account.h"
#include "xf_contact.h"
#include "xf_protocol.h"

#define XF_HEARTBEAT_TIMEOUT (30 * 1000)
#define XF_CONNECTION_TIMEOUT (XF_HEARTBEAT_TIMEOUT + 30 * 1000)

class XfireServer : public QObject
{
    Q_OBJECT;

public:
    // Constructor and destructor
    XfireServer(XfireAccount *parent);
    ~XfireServer();

    void connectToServer(const QString p_account, const QString p_password, const QString p_serverIp, const uint p_serverPort);

    // Xfire server and port
    QString serverName() const;
    uint serverPort() const;

    // Packets sent to the server
    void sendAskFriendExtendedInformation(quint32 p_uid);
    void sendChat(const Xfire::SessionID &p_sid, quint32 p_chatMessageIndex, const QString &p_message);
    void sendChatConfirmation(const Xfire::SessionID &p_sid, quint32 p_messageIndex);
    void sendFriendInvitation(QString &p_username, QString &p_message);
    void sendFriendInvitationResponse(QString p_username, bool p_response);
    void sendFriendNetworkRequest(QList<Xfire::SIDAttribute*> p_fofs);
    void sendIngameStatus(quint32 p_gameId, quint32 p_ip, quint32 p_port);
    void sendNickname(const QString &p_nickname);
    void sendP2pSession(const Xfire::SessionID &p_sid, quint32 p_ip, quint16 p_port, quint32 p_localIp, quint16 p_localPort, quint32 p_natType, const QString &p_salt);
    void sendStatusMessage(const QString &p_message);
    void sendTypingStatus(const Xfire::SessionID &p_sid, quint32 p_chatMessageIndex, bool p_isTyping);
    void sendFriendRemoval(quint32 p_uid);

    QTcpSocket *m_connection;

    QString m_salt; // FIXME: Private or public?

private:
    XfireAccount *m_account;

    // Login information
    QString m_username;
    QString m_password;

    // Connection
    QByteArray m_buffer;
    QTimer *m_heartBeat;
    QTimer *m_connectionTimeout;

public slots:
    void closeConnection();
    void handlePacket(const Xfire::Packet *p_packet, XfireP2PSession *p_session = 0);

private slots:
    void slotConnected();
    void socketRead();
    void login(const QString &p_salt);
    void slotSendHeartBeat();
    void slotConnectionInterrupted(QAbstractSocket::SocketError p_error = QAbstractSocket::UnknownSocketError);
    void slotAddedInfoEventActionActivated(uint p_actionId);

signals:
    void goOffline();
    void goOnline();

    void addUser(const QString &p_contactId, const QString &p_displayName, int p_groupId);
    void ourStatusChanged(const Kopete::OnlineStatus &p_status);
};

#endif // XF_SERVER_H
