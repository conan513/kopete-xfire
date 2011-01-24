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

#ifndef XF_P2P_H
#define XF_P2P_H

#define XFIRE_P2P_TYPE_DATA32           0x0000
#define XFIRE_P2P_TYPE_PING             0x0010
#define XFIRE_P2P_TYPE_PONG             0x0020
#define XFIRE_P2P_TYPE_ACK              0x0040
#define XFIRE_P2P_TYPE_BADCRC           0x0080
#define XFIRE_P2P_TYPE_DATA16           0x0300
#define XFIRE_P2P_TYPE_KEEP_ALIVE_REQ   0x0800
#define XFIRE_P2P_TYPE_KEEP_ALIVE_REP   0x1000

#include "XfireAttribute.h"
#include "XfirePeerToPeerPacket.h"
#include "XfireTypes.h"

#define XF_P2P_FT_CHUNK_SIZE 0xC800 // 50 * 1024 bytes = 51200 bytes

#include <QUdpSocket>

class XfireAccount;
class XfireP2PSession;
class XfireP2PNatcheck;

class XfireP2P : public QObject
{
    Q_OBJECT

public:
    // Constructor & destructor
    XfireP2P(XfireAccount *p_account);
    ~XfireP2P();

    XfireAccount *m_account;

    quint32 m_messageId;
    quint32 m_sessionId;

    QUdpSocket *m_connection;
    XfireP2PNatcheck *m_natCheck;

    QByteArray createHeader(quint8 p_encoding, QByteArray p_moniker, quint32 p_type, quint32 p_messageId, quint32 p_sequenceId, quint32 p_dataLen);
    void sendPacket(XfireP2PSession *p_session, QByteArray &p_data);

    void sendPing(XfireP2PSession *p_session);
    void sendPong(XfireP2PSession *p_session);
    void sendKeepAlive(XfireP2PSession *p_session);
    void sendKeepAliveRequest(XfireP2PSession *p_session);
    void sendAck(XfireP2PSession *p_session, quint32 p_sessionId, quint32 p_sequenceId);
    void sendData16( XfireP2PSession *p_session, quint32 p_sequenceId, quint8 p_encoding, QByteArray p_data, const char *p_category);
    void sendData32(XfireP2PSession *p_session, quint32 p_sequenceId, quint8 p_encoding, QByteArray p_data, const char *p_category);
    void sendBadCrc32(XfireP2PSession *p_session, quint32 p_sequenceId);

private:
    quint32 calculateCrc32(const char *p_data, quint32 p_len);
    quint32 m_transferMessageId; // FIXME: Does not belong here!

public slots:
    void slotSocketRead();
    void slotNatCheckReady(QUdpSocket *p_connection);
};

#endif // XF_P2P_H
// kate: indent-mode cstyle; space-indent on; indent-width 0;
