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

#include <KDebug>

#include "xf_account.h"
#include "xf_p2p.h"
#include "xf_p2p_natcheck.h"
#include "xf_p2p_session.h"
#include "xf_contact.h"
#include "xf_server.h"

XfireP2P::XfireP2P(XfireAccount *p_account)
{
    m_account = p_account;

    // Start NAT type check
    m_natCheck = new XfireP2PNatcheck(this);
    connect(m_natCheck, SIGNAL(ready()), this, SLOT(slotNatCheckReady()));
}

XfireP2P::~XfireP2P()
{
}

bool XfireP2P::isConnected()
{
    return (m_connection->isOpen());
}

void XfireP2P::addSession(XfireP2PSession *p_session)
{
    m_sessions.append(p_session);
}

void XfireP2P::slotSocketRead()
{
    QByteArray datagram;
    datagram.resize(m_connection->pendingDatagramSize());

    QHostAddress sender;
    quint16 port; // FIXME: Not needed?

    m_connection->readDatagram(datagram.data(), datagram.size(), &sender, &port);

    if(datagram.size() < 44)
    {
        kDebug() << "Bad P2P packet received, ignoring";
        return;
    }

    // Get moniker
    QByteArray moniker = datagram.mid(4, 20);
    XfireP2PSession *session = m_account->p2pSessionByMoniker(moniker);

    if(!session)
    {
        kDebug() << "Unknown P2P session, ignoring";
        return;
    }

    quint32 type;
    memcpy(&type, datagram.data() + 24, 4);

    switch (type)
    {
    case XFIRE_P2P_TYPE_DATA32:
    {
        kDebug() << "Received data32 packet";
        break;
    }
    case XFIRE_P2P_TYPE_PING:
    {
        kDebug() << "Received ping packet";
        sendPong(session);
        break;
    }
    case XFIRE_P2P_TYPE_PONG:
    {
        kDebug() << "Received pong packet";
        break;
    }
    case XFIRE_P2P_TYPE_ACK:
    {
        kDebug() << "Received ack packet";
        break;
    }
    case XFIRE_P2P_TYPE_BADCRC:
    {
        kDebug() << "Received badcrc packet";
        break;
    }
    case XFIRE_P2P_TYPE_DATA16:
    {
        kDebug() << "Received data16 packet";
        break;
    }
    case XFIRE_P2P_TYPE_KEEP_ALIVE_REQ:
    {
        kDebug() << "Received keep alive request packet";
        break;
    }
    case XFIRE_P2P_TYPE_KEEP_ALIVE_REP:
    {
        kDebug() << "Received keep alive reply packet";
        break;
    }
    default:
    {
        kDebug() << "Received unknown packet type";
        break;
    }
    }
}

QByteArray XfireP2P::createHeader(quint8 p_encoding, QByteArray p_moniker, quint32 p_type, quint32 p_messageId, quint32 p_sequenceId, quint32 p_dataLen)
{
    QByteArray ret;

    ret.append(p_encoding); // Encoding
    ret.append(QByteArray().fill((char )0, 3)); // 3 unknown bytes
    ret.append(p_moniker); // Moniker

    // Type
    ret.append(p_type);
    ret.append(QByteArray().fill((char )0, 3));

    // Message ID
    ret.append(QByteArray::number(p_messageId));
    ret.append(QByteArray().fill((char )0, 3));

    // Sequence ID
    ret.append(p_sequenceId);
    ret.append(QByteArray().fill((char )0, 3));

    // Data length
    ret.append(p_dataLen);
    ret.append(QByteArray().fill((char )0, 3));

    ret.append(QByteArray().fill((char )0, 4)); // 3 unknown bytes

    kDebug() << ret.toHex();

    return ret;
}

void XfireP2P::sendPing(XfireP2PSession *p_session)
{
    QByteArray foo = createHeader(0, p_session->m_moniker, XFIRE_P2P_TYPE_PING, (p_session->m_sessionId > 0) ? p_session->m_sessionId : p_session->m_contact->m_account->m_p2pConnection->m_messageId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::sendPong(XfireP2PSession *p_session)
{
    kDebug() << "Sending pong packet to: " + QHostAddress(p_session->m_remoteIp).toString() + ":" << QString::number(p_session->m_remotePort);
    QByteArray foo = createHeader(0, p_session->m_moniker, XFIRE_P2P_TYPE_PONG, (p_session->m_sessionId > 0) ? p_session->m_sessionId : p_session->m_contact->m_account->m_p2pConnection->m_messageId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::slotNatCheckReady()
{
    m_connection = new QUdpSocket(this);
    m_connection->bind();

    connect(m_connection, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));
    kDebug() << "Peer to peer connection started on port" << m_connection->localPort();
}
