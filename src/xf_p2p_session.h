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
	// Constructor and destructor

	XfireP2PSession(XfireContact *pContact, QString pSalt);
	~XfireP2PSession();

	XfireContact *mContact;

	QByteArray monikerSelf;

	// Peer remote IP & port
	quint32 remoteIP;
	quint16 remotePort;

	// Peer local IP & port
	quint32 localIP;
	quint16 localPort;

	void setRemoteAddress(quint32 pIP, quint16 pPort);
	void setLocalAddress(quint32 pIP, quint16 pPort);

	quint32 sessid;
	quint32 seqid;
	quint32 msgid;
};

#endif // XF_P2P_SESSION_H
