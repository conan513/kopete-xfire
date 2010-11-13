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

#include <XfireAttribute.h>

class XfireContact;

class XfireP2PSession
{
public:
	// Constructor & destructor
	XfireP2PSession(XfireContact *p_contact, QString p_salt);
	~XfireP2PSession();

	XfireContact *m_contact;
	QByteArray m_moniker;

	// Peer local/remote IP & port
	quint32 m_localIp;
	quint16 m_localPort;
	quint32 m_remoteIp;
	quint16 m_remotePort;

	void setLocalAddress(quint32 p_ip, quint16 p_port);
	void setRemoteAddress(quint32 p_ip, quint16 p_port);

	quint32 m_sessionId;
	quint32 m_sequenceId;
	quint32 m_messageId;
};

#endif // XF_P2P_SESSION_H
