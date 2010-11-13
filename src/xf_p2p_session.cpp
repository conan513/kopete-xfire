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

XfireP2PSession::XfireP2PSession(XfireContact* p_contact, QString p_salt)
{
	m_contact = p_contact;

	// Generate moniker
	QCryptographicHash hasher(QCryptographicHash::Sha1);

	QByteArray hash;
	hasher.addData(p_contact->m_session.raw().toHex());
	hasher.addData(p_salt.toAscii());

	m_moniker = hasher.result();
	kDebug() << "Moniker generated:" << p_contact->contactId() << "->" << m_moniker << m_moniker.toHex();
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
}
