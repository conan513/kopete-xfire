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

XfireP2PSession::XfireP2PSession(XfireContact* pContact, QString pSalt)
{
	mContact = pContact;

	// Generate moniker
	QCryptographicHash hasher(QCryptographicHash::Sha1);

	QByteArray hash;
	hasher.addData(pContact->mSession.raw().toHex());
	hasher.addData(pSalt.toAscii());

	monikerSelf = hasher.result();
	kDebug() << "Generate moniker of:" << pContact->contactId() << ":" << monikerSelf;
	kDebug() << "Hexed:" << monikerSelf.toHex();
}

XfireP2PSession::~XfireP2PSession()
{
}

void XfireP2PSession::setLocalAddress(quint32 pIP, quint16 pPort)
{
	localIP = pIP;
	localPort = pPort;
}

void XfireP2PSession::setRemoteAddress(quint32 pIP, quint16 pPort)
{
	remoteIP = pIP;
	remotePort = pPort;
}
