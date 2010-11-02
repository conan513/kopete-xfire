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
#include "xf_p2p.h"

#include <QSysInfo>

XfireP2P::XfireP2P(XfireAccount *pAccount)
{
	mAccount = pAccount;

	mConnection = new QUdpSocket(this);
	mConnection->bind();

	connect(mConnection, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));

	kDebug() << "Connection open on port" << mConnection->localPort();
}

XfireP2P::~XfireP2P()
{
}

bool
XfireP2P::isConnected()
{
	return (mConnection->isOpen());
}

void
XfireP2P::addSession(XfireP2PSession *pSession)
{
	mSessions.append(pSession);
}

void
XfireP2P::slotSocketRead()
{
	kDebug() << "Received P2P packet.";

	QByteArray datagram;
	datagram.resize(mConnection->pendingDatagramSize());

	QHostAddress sender;
	quint16 senderPort;

	mConnection->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

	if(datagram.size() < 44)
	{
		kDebug() << "Bad P2P packet received, ignoring.";
		return;
	}

	// Get moniker
	QByteArray moniker = datagram.mid(4, 20);
	XfireP2PSession *session = mAccount->p2pSessionByMoniker(moniker);

	if(!session)
	{
		kDebug() << "Unknown P2P session, ignoring.";
		return;
	}

	quint32 type;
	memcpy(&type, datagram.data() + 24, 4);

	switch(type)
	{
	case XFIRE_P2P_TYPE_PING:
		{
			kDebug() << "Received P2P ping packet.";
			sendPing(session);
			break;
		}
	case XFIRE_P2P_TYPE_PONG:
		{
			kDebug() << "Received P2P pong packet.";
			break;
		}
	default:
		{
			kDebug() << "Unkown P2P packet type.";
			break;
		}
	}
}

QByteArray
XfireP2P::createHeader(quint8 pEncoding, QByteArray pMoniker, quint32 pType, quint32 pMsgID, quint32 pSeqID, quint32 pDataLen)
{
	QByteArray ret;

	ret.append(pEncoding); // Encoding
	ret.append(QByteArray().fill((char )0, 3)); // 3 unknown bytes
	ret.append(pMoniker); // Moniker

	// Type
	ret.append(pType);
	ret.append(QByteArray().fill((char )0, 3));

	// Message ID
	ret.append(QByteArray::number(pMsgID));
	ret.append(QByteArray().fill((char )0, 3));

	// Sequence ID
	ret.append(pSeqID);
	ret.append(QByteArray().fill((char )0, 3));

	// Data length
	ret.append(pDataLen);
	ret.append(QByteArray().fill((char )0, 3));

	ret.append(QByteArray().fill((char )0, 4)); // 3 unknown bytes

	kDebug() << ret.toHex();

	return ret;
}

void
XfireP2P::sendPing(XfireP2PSession *pSession)
{
	QByteArray foo = createHeader(0, pSession->monikerSelf, XFIRE_P2P_TYPE_PING, (pSession->sessid > 0) ? pSession->sessid : pSession->msgid, 0, 0);
	mConnection->writeDatagram(foo, QHostAddress(pSession->remoteIP), pSession->remotePort);
}

