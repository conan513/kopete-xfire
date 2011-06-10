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

#ifndef XF_P2P_SESSION_H
#define XF_P2P_SESSION_H

#include <QString>
#include <QTime>
#include <QTimer>

#include <XfireAttribute.h>

class XfireContact;
class XfireP2PFileTransfer;
class XfireP2P;

class XfireP2PSession : public QObject
{
    Q_OBJECT;

public:
    // Constructor & destructor
    XfireP2PSession(XfireContact* p_contact, const QString& p_salt);
    ~XfireP2PSession();

    XfireContact *m_contact;

    XfireP2P *m_p2p;
    QByteArray m_moniker;
    QByteArray m_monikerSelf;

    // Peer local/remote IP & port
    quint32 m_localIp;
    quint16 m_localPort;
    quint32 m_remoteIp;
    quint16 m_remotePort;
    int m_natType;

    void setLocalAddress(quint32 p_ip, quint16 p_port);
    void setRemoteAddress(quint32 p_ip, quint16 p_port);

    bool m_pongNeeded;
    bool m_keepAliveNeeded;
    bool m_handshakeDone;
    bool m_triedLocalAddress;

    quint32 m_sequenceId;
    QHash<quint32, XfireP2PFileTransfer*> m_fileTransfers;

    void createFileTransfer(quint32 p_fileid, const QString& p_filename, quint64 p_size);
    
    void sendMessage(quint32 p_chatMessageIndex, const QString &p_message);
    void sendMessageConfirmation(quint32 p_chatMessageIndex);
    void sendTypingStatus(quint32 p_chatMessageIndex, bool p_isTyping);
    void sendFileRequestReply(quint32 p_fileid, bool p_reply);
    void sendFileChunkInfoRequest(quint32 p_fileid, quint64 p_offset, quint32 p_chunkSize, quint32 p_chunkCount, quint32 p_messageId);
    void sendFileDataPacketRequest(quint32 p_fileid, quint64 p_offset, quint32 p_size, quint32 p_messageId);
    void sendFileComplete(quint32 p_fileid);

private:
    QTimer *m_timer;
    QTime *m_lastKeepAlive;
    QTime *m_lastPing;

    int m_pingRetries;

private slots:
    void slotCheckSession();
    void slotFileTransferReady(XfireP2PFileTransfer* p_fileTransfer);

signals:
    void timeout();
};

#endif // XF_P2P_SESSION_H
