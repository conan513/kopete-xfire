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

#include <QObject>

#include "xf_account.h"
#include "xf_p2p_session.h"
#include "xf_p2p_natcheck.h"

// P2P packet types
#define XFIRE_P2P_TYPE_DATA32			0x0000
#define XFIRE_P2P_TYPE_PING				0x0010
#define XFIRE_P2P_TYPE_PONG				0x0020
#define XFIRE_P2P_TYPE_ACK				0x0040
#define XFIRE_P2P_TYPE_BADCRC			0x0080
#define XFIRE_P2P_TYPE_DATA16			0x0300
#define XFIRE_P2P_TYPE_KEEP_ALIVE_REQ	0x0800
#define XFIRE_P2P_TYPE_KEEP_ALIVE_REP	0x1000

class XfireP2P : public QObject
{
	Q_OBJECT

	public:
		// Constructor & destructor
		XfireP2P(XfireAccount *p_account);
		~XfireP2P();

		XfireAccount *m_account;

		QUdpSocket *m_connection;
		QList<XfireP2PSession *> m_sessions;
		XfireP2PNatcheck *m_natCheck;

		bool isConnected();
		void addSession(XfireP2PSession *p_session);

		QByteArray createHeader(quint8 p_encoding, QByteArray p_moniker, quint32 p_type, quint32 p_messageId, quint32 p_sequenceId, quint32 p_dataLen);
		void sendPing(XfireP2PSession *p_session);

	public slots:
		void slotSocketRead();
		void slotNatCheckReady();
};

#endif // XF_P2P_H
