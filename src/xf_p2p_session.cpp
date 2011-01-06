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

XfireP2PSession::XfireP2PSession(XfireContact *p_contact, const QString &p_salt) : QObject(p_contact), m_contact(p_contact), m_pingRetries(0)
{
    // Generate moniker
    QCryptographicHash hasher(QCryptographicHash::Sha1);

    hasher.addData(p_contact->m_session.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_moniker = hasher.result();
    kDebug() << p_contact->m_username + ": moniker generated: " + m_moniker.toHex();

    hasher.reset();
    hasher.addData(p_contact->m_account->m_sid.raw().toHex());
    hasher.addData(p_salt.toAscii());
    m_monikerSelf = hasher.result();
    kDebug() << "Moniker generated: " + m_monikerSelf.toHex();
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
	// Check pong timeout
	if(m_pongNeeded && m_lastPing->restart() >= 5000)
	{
		if(m_pingRetries++ < 5)
		{
			kDebug() << "Resending ping packet to:" << m_contact->m_username << "1/" << m_pingRetries;
			m_contact->m_account->m_p2pConnection->sendPing(this); // FIXME: ugly
			m_pongNeeded = TRUE;
		}
		else
		{
			kDebug() << m_contact->m_username + ": session timed out, removing session";
			m_contact->m_account->m_p2pConnection->removeSession(this);
			m_timer->stop();
		}
	}

	// Check keep-alive timeout
	// ...

	// Request keep-alive
	if(!m_keepAliveNeeded && m_lastKeepAlive->restart() >= 60)
	{
		kDebug() << "Send keep-alive request packet to:" << m_contact->m_username;
		m_contact->m_account->m_p2pConnection->sendKeepAliveRequest(this);
	}
}
