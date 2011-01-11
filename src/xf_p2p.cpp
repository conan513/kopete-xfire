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

#include <stdio.h>
#include <stdlib.h>

#include <KDebug>

#include "xf_account.h"
#include "xf_p2p.h"
#include "xf_p2p_natcheck.h"
#include "xf_p2p_session.h"
#include "xf_contact.h"
#include "xf_server.h"

XfireP2P::XfireP2P(XfireAccount *p_account): m_account(p_account), m_messageId(0), m_sessionId(0)
{
    // Start NAT type check
    m_natCheck = new XfireP2PNatcheck(this);
    connect(m_natCheck, SIGNAL(ready(QUdpSocket *)), this, SLOT(slotNatCheckReady(QUdpSocket *)));
}

XfireP2P::~XfireP2P()
{
}

void XfireP2P::slotSocketRead()
{
    QByteArray datagram;
    datagram.resize(m_connection->pendingDatagramSize());

    QHostAddress sender;
    quint16 port;

    m_connection->readDatagram(datagram.data(), datagram.size(), &sender, &port);

    if(datagram.size() < 44)
    {
        kDebug() << "Bad P2P packet received, ignoring";
        return;
    }

    quint8 encoding = *datagram.data();

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

    quint32 messageId;
    memcpy(&messageId, datagram.data() + 28, 4);

    quint32 sequenceId;
    memcpy(&sequenceId, datagram.data() + 32, 4);

    switch(type)
    {
    case XFIRE_P2P_TYPE_PING:
    {
        kDebug() << "Received ping packet";

        // NAT type 2: incorrect port received in p2p data
        if(session->m_natType == 2)
            session->m_contact->m_p2pSession->setRemoteAddress(sender.toIPv4Address(), port);

        sendPong(session);
        break;
    }
    case XFIRE_P2P_TYPE_PONG:
    {
        kDebug() << "Received pong packet";
        session->m_pongNeeded = false;
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
    case XFIRE_P2P_TYPE_DATA32:
    {
        quint32 size;
        memcpy(&size, datagram.constData() + 36, 4);

        kDebug() << "Received data packet (" << size << " bytes)";

        if(datagram.size() < (68 + size))
        {
            kDebug() << "Received too short data packet";
            break;
        }

        // Decode
        char *crc_data = datagram.data() + 44;

        if(encoding != 0x00)
        {
            kDebug() << "Decoding encoded packet";

            for(quint32 i = 0; i < (4 + size + 16 + 4); i++)
                *(crc_data + i) ^= encoding;
        }

        // Category
        char category[17];
        category[16] = 0;
        memcpy(category, datagram.constData() + 48 + size, 16);
        kDebug() << "Packet category:" << category;

        // CRC32 check
        quint32 crc32;
        memcpy(&crc32, datagram.constData() + 48 + size + 16, 4);

        // FIXME: not tested
        if(crc32 != calculateCrc32(crc_data, (48 + size + 16) - 44))
        {
            kDebug() << "Received packet with invalid CRC-32";
            sendBadCrc32(session, session->m_sequenceId);
            break;
        }

        // Handle data
        if(type == XFIRE_P2P_TYPE_DATA16)
        {
            if(!strcmp(category, "IM"))
            {
                kDebug() << "Received im packet";

                // Get data into Xfire packet
                Xfire::Packet *packet = Xfire::Packet::parseData(datagram.mid(40 + 8, size)); // 8 unknown bytes
                if(packet && packet->isValid())
                    m_account->server()->handlePacket(packet, session);
            }
        }
        else
        {
            if(!strcmp(category, "DL"))
            {
                kDebug() << "Received dl packet";

                quint16 type;
                memcpy(&type, datagram.constData() + 40 + 8 + 4, 2);

                switch(type)
                {
                    case 0x3E87:
                    {
                        kDebug() << "Received file request";
                        // FIXME: not implemented yet
                        break;
                    }
                    default:
                        kDebug() << "Invalid dl packet received"; // FIXME: don't send ack then
                }
            }
        }

        // Acknowledge packet
        sendAck(session, messageId, sequenceId);

        break;
    }
    case XFIRE_P2P_TYPE_KEEP_ALIVE_REQ:
    {
        kDebug() << "Received keep-alive request packet";
        sendKeepAlive(session);
        break;
    }
    case XFIRE_P2P_TYPE_KEEP_ALIVE_REP:
    {
        kDebug() << "Received keep-alive reply packet";
        session->m_keepAliveNeeded = false;
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

    ret.append((char )p_encoding); // Encoding
    ret.append(QByteArray().fill(0, 3)); // 3 unknown bytes
    ret.append(p_moniker); // Moniker
    ret.append((char *)&p_type, 4); // Type
    ret.append((char *)&p_messageId, 4); // Message ID
    ret.append((char *)&p_sequenceId, 4); // Sequence ID
    ret.append((char *)&p_dataLen, 4); // Data length
    ret.append(QByteArray().fill(0, 4)); // 3 unknown bytes

    return ret;
}

void XfireP2P::sendPing(XfireP2PSession *p_session)
{
    kDebug() << "Sending ping packet to: " << QHostAddress(p_session->m_remoteIp).toString() << ":" << QString::number(p_session->m_remotePort);
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_PING,(m_sessionId > 0)? m_sessionId : m_messageId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);

    if(m_sessionId == 0)
    {
        m_messageId++;
        m_sessionId = m_messageId - 1;
    }
    else
        m_sessionId++;
}

void XfireP2P::sendPong(XfireP2PSession *p_session)
{
    kDebug() << "Sending pong to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_PONG,(m_sessionId > 0)? m_sessionId : m_messageId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);

    if(m_sessionId == 0)
    {
        m_messageId++;
        m_sessionId = m_messageId - 1;
    }
    else
        m_sessionId++;
}

void XfireP2P::sendKeepAlive(XfireP2PSession *p_session)
{
    kDebug() << "Sending keep-alive to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_KEEP_ALIVE_REP, m_sessionId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::sendKeepAliveRequest(XfireP2PSession *p_session)
{
    kDebug() << "Sending keep-alive request to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_KEEP_ALIVE_REQ, m_sessionId, 0, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::sendAck(XfireP2PSession *p_session, quint32 p_sessionId, quint32 p_sequenceId)
{
    kDebug() << "Sending ack to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_ACK, p_sessionId, p_sequenceId, 0);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::sendData16(XfireP2PSession *p_session, quint32 p_sequenceId, quint8 p_encoding, QByteArray p_data, const char *p_category)
{
    kDebug() << "Sending data16 to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(p_encoding, p_session->m_monikerSelf, XFIRE_P2P_TYPE_DATA16, m_messageId, p_sequenceId, p_data.size());

    quint32 offset = foo.size();

    foo.append(QByteArray(4, 0)); // 4 unknown bytes
    foo.append(p_data); // Data
    foo.append(p_category, strlen(p_category)); // Category
    foo.append(QByteArray(16 - strlen(p_category), 0)); // Fill up to 16 bytes

    // CRC32
    quint32 crc32 = calculateCrc32(foo.constData() + offset, 4 + p_data.size() + 16);
    foo.append((const char*)&crc32, 4);

    // Encode data
    if(p_encoding != 0) // FIXME: not tested
    {
        char *checksumData = foo.data();
        while(checksumData < (foo.data() + offset + 4))
        {
            *checksumData ^= p_encoding;
            checksumData++;
        }
    }

    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);

    m_messageId++;
    p_session->m_sequenceId++;
}

void XfireP2P::sendBadCrc32(XfireP2PSession *p_session, quint32 p_sequenceId)
{
    kDebug() << "Sending badcrc to:" << p_session->m_contact->m_username;
    QByteArray foo = createHeader(0, p_session->m_monikerSelf, XFIRE_P2P_TYPE_BADCRC, m_sessionId, m_messageId, p_sequenceId);
    m_connection->writeDatagram(foo, QHostAddress(p_session->m_remoteIp), p_session->m_remotePort);
}

void XfireP2P::slotNatCheckReady(QUdpSocket *p_connection)
{
    m_connection = p_connection;

    connect(m_connection, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));
    kDebug() << "Peer to peer connection started on port" << m_connection->localPort();
}

static const quint32 crc32Table[256] =
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

quint32 XfireP2P::calculateCrc32(const char *p_data, quint32 p_len)
{
    quint32 crc32 = 0xFFFFFFFF;

    for(quint32 i = 0; i < p_len; i++)
        crc32 = (crc32 >> 8) ^ crc32Table[((quint8* )p_data)[i] ^ (crc32 & 0x000000FF)];

    return ~crc32;
}
