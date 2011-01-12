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

#include "xf_contact.h"
#include "xf_p2p_session.h"
#include "xf_server.h"

XfireP2PSession::XfireP2PSession(XfireContact *p_contact, const QString &p_salt) : QObject(p_contact), m_contact(p_contact), m_pingRetries(0), m_natType(0), m_sequenceId(0)
{
    kDebug() << m_contact->m_username + ": creating p2p session";

    // Generate monikers (self and peer)
    QCryptographicHash hasher(QCryptographicHash::Sha1);

    hasher.addData(p_contact->m_sid.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_moniker = hasher.result();

    hasher.reset();
    hasher.addData(p_contact->m_account->m_sid.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_monikerSelf = hasher.result();

    kDebug() << "Moniker generated: " + m_monikerSelf.toHex();
    kDebug() << "Peer moniker generated:" << p_contact->m_username + ": " << m_moniker.toHex();
}

XfireP2PSession::~XfireP2PSession()
{
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

	m_timer = new QTimer(this);
	m_timer->start(1000);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotCheckSession()));
}

void XfireP2PSession::slotCheckSession()
{
}

void XfireP2PSession::sendMessage( quint32 p_chatMessageIndex, const QString &p_message)
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