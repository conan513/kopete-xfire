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
#include "xf_contact.h"
#include "xf_p2p.h"
#include "xf_p2p_filetransfer.h"
#include "xf_p2p_session.h"
#include "xf_server.h"

XfireP2PSession::XfireP2PSession(XfireContact *p_contact, const QString &p_salt) : QObject(p_contact),
    m_contact(p_contact), m_p2p(p_contact->m_account->m_p2pConnection), m_pingRetries(0), m_natType(0), m_sequenceId(0), m_handshakeDone(FALSE), m_triedLocalAddress(FALSE)
{
    kDebug() << m_contact->m_username + ": creating P2P session";

    // Generate monikers (self and peer)
    QCryptographicHash hasher(QCryptographicHash::Sha1);

    hasher.addData(p_contact->m_sid.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_moniker = hasher.result();

    hasher.reset();
    hasher.addData(p_contact->m_account->m_sid.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_monikerSelf = hasher.result();

    kDebug() << "Moniker: " + m_monikerSelf.toHex();
    kDebug() << "Peer moniker: " + p_contact->m_username + ": " + m_moniker.toHex();
}

XfireP2PSession::~XfireP2PSession()
{
    kDebug() << m_contact->m_username + ": removing P2P session";

    delete m_lastPing;
    delete m_lastKeepAlive;
    m_timer->stop();
}

void XfireP2PSession::setLocalAddress(quint32 p_ip, quint16 p_port)
{
    m_localIp = p_ip;
    m_localPort = p_port;
}

void XfireP2PSession::setRemoteAddress(quint32 p_ip, quint16 p_port)
{
    m_remoteIp = p_ip;
    m_remotePort = p_port;

    m_lastPing = new QTime();
    m_lastKeepAlive = new QTime();

    m_timer = new QTimer(this);
    m_timer->start(1000);

    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotCheckSession()));
    m_contact->m_account->m_p2pConnection->sendPing(this);
}

void XfireP2PSession::slotCheckSession()
{
    // Check pong timeout
    if(m_pongNeeded && m_lastPing->elapsed() >= 50000)
    {
        if(m_pingRetries++ < 10)
        {
            m_contact->m_account->m_p2pConnection->sendPing(this);
            m_lastPing->restart();
        }
        else
            emit timeout();
    }

    // Check keep-alive timeout
    /*if(m_keepAliveNeeded && m_lastKeepAlive->elapsed() >= 10000)
        emit timeout();

    // Request keep-alive
    if(!m_keepAliveNeeded && m_lastKeepAlive->elapsed() >= 60000)
    {
        m_contact->m_account->m_p2pConnection->sendKeepAliveRequest(this);
        m_lastKeepAlive->restart();
    }*/
}

void XfireP2PSession::createFileTransfer(quint32 p_fileid, const QString &p_filename, quint64 p_size)
{
    XfireP2PFileTransfer *ft = new XfireP2PFileTransfer(this, p_fileid, p_filename, p_size);
    m_fileTransfers.insert(p_fileid, ft);

    connect(ft, SIGNAL(ready(XfireP2PFileTransfer*)), this, SLOT(slotFileTransferReady(XfireP2PFileTransfer*)));
}

void XfireP2PSession::slotFileTransferReady(XfireP2PFileTransfer* p_fileTransfer)
{
    kDebug() << "File transfer completed: fileid:" << p_fileTransfer->m_fileid;
    m_fileTransfers.remove(p_fileTransfer->m_fileid);
    delete p_fileTransfer;
}

void XfireP2PSession::sendMessage(quint32 p_chatMessageIndex, const QString &p_message)
{
    Xfire::Packet foo(0x0002);
    foo.addAttribute(new Xfire::SIDAttributeS("sid", m_contact->m_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 0));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    peermsg->addAttribute(new Xfire::StringAttributeS("im", p_message));
    foo.addAttribute(peermsg);

    m_contact->m_account->m_p2pConnection->sendData16(this, m_sequenceId, 0, foo.toByteArray(), "IM");
}

void XfireP2PSession::sendMessageConfirmation(quint32 p_chatMessageIndex)
{
    Xfire::Packet foo(0x0002);
    foo.addAttribute(new Xfire::SIDAttributeS("sid", m_contact->m_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 1));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    foo.addAttribute(peermsg);

    m_contact->m_account->m_p2pConnection->sendData16(this, m_sequenceId, 0, foo.toByteArray(), "IM");
}

void XfireP2PSession::sendTypingStatus(quint32 p_chatMessageIndex, bool p_isTyping)
{
    Xfire::Packet foo(0x0002);
    foo.addAttribute(new Xfire::SIDAttributeS("sid", m_contact->m_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 3));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    peermsg->addAttribute(new Xfire::Int32AttributeS("typing", p_isTyping ? 1 : 0));
    foo.addAttribute(peermsg);

    m_contact->m_account->m_p2pConnection->sendData16(this, m_sequenceId, 0, foo.toByteArray(), "IM");
}

void XfireP2PSession::sendFileRequestReply(quint32 p_fileid, bool p_reply)
{
    Xfire::PeerToPeerPacket foo(0x3E88);
    foo.addAttribute(new Xfire::Int32AttributeS("fileid", p_fileid));
    foo.addAttribute(new Xfire::BoolAttributeS("reply", p_reply));
    
    m_contact->m_account->m_p2pConnection->sendData32(this, m_sequenceId, 0, foo.toByteArray(), "DL");
}

void XfireP2PSession::sendFileChunkInfoRequest(quint32 p_fileid, quint64 p_offset, quint32 p_chunkSize, quint32 p_chunkCount, quint32 p_messageId)
{
    Xfire::PeerToPeerPacket foo(0x3E89);
    foo.addAttribute(new Xfire::Int32AttributeS("fileid", p_fileid));
    foo.addAttribute(new Xfire::Int64AttributeS("offset", p_offset));
    foo.addAttribute(new Xfire::Int32AttributeS("size", p_chunkSize));
    foo.addAttribute(new Xfire::Int32AttributeS("chunkcnt", p_chunkCount));
    foo.addAttribute(new Xfire::Int32AttributeS("msgid", p_messageId));
    
    m_contact->m_account->m_p2pConnection->sendData32(this, m_sequenceId, 0, foo.toByteArray(), "DL");
}

void XfireP2PSession::sendFileDataPacketRequest(quint32 p_fileid, quint64 p_offset, quint32 p_size, quint32 p_messageId)
{
    Xfire::PeerToPeerPacket foo(0x3E8B);
    foo.addAttribute(new Xfire::Int32AttributeS("fileid", p_fileid));
    foo.addAttribute(new Xfire::Int64AttributeS("offset", p_offset));
    foo.addAttribute(new Xfire::Int32AttributeS("size", p_size));
    foo.addAttribute(new Xfire::Int32AttributeS("msgid", p_messageId));
    
    m_contact->m_account->m_p2pConnection->sendData32(this, m_sequenceId, 0, foo.toByteArray(), "DL");
}