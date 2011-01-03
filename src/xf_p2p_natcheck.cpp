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
#include "xf_p2p_natcheck.h"

XfireP2PNatcheck::XfireP2PNatcheck(XfireP2P *p_p2p)
{
    m_p2p = p_p2p;

    m_connection = new QUdpSocket(this);
    m_connection->bind();

    connect(m_connection, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));

    // Get servers
    // FIXME: Use DNS instead of hard-coding
    m_servers[0] = "208.88.178.60";
    m_servers[1] = "208.88.178.61";
    m_servers[2] = "208.88.178.62";

    m_stage = 0;
    m_multiplePorts = FALSE;

    // Send datagrams to servers
    QByteArray foo = QByteArray::fromHex("53433031045acb0001000000000000000000");
    m_connection->writeDatagram(foo, QHostAddress(m_servers[0]), XFIRE_NAT_PORT);
    m_connection->writeDatagram(foo, QHostAddress(m_servers[1]), XFIRE_NAT_PORT);
    m_connection->writeDatagram(foo, QHostAddress(m_servers[2]), XFIRE_NAT_PORT);
}

XfireP2PNatcheck::~XfireP2PNatcheck()
{
}

void XfireP2PNatcheck::slotSocketRead()
{
    qint64 size = m_connection->pendingDatagramSize();
    if (size != 10)
    {
        kDebug() << "Invalid packet received, ignoring.";
        return;
    }

    QByteArray datagram;
    datagram.resize(size);

    QHostAddress sender;
    quint16 senderPort;

    m_connection->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    // Check for eventually unknown server
    int i;
    for (i = 0; i < 3; i++)
    {
        if (sender.toString() == m_servers[i])
            break;
    }

    if (i == 3)
    {
        kDebug() << "Received packet from unknown server, aborting.";
        return;
    }

    // Check magic
    if (datagram.mid(0, 4) != "CK01")
    {
        kDebug() << "Received packet with bad magic, aborting.";
        return;
    }

    if (senderPort != XFIRE_NAT_PORT)
        m_multiplePorts = TRUE;

    quint32 ip = *((quint32*)(datagram.constData() + 4));
    quint32 port = *((quint16*)(datagram.constData() + 8));

    kDebug() << "Server " + sender.toString() + " reported: " + QHostAddress(ip).toString();

    m_ips[i] = ip;
    m_ports[i] = port;

    //Determine NAT type when stage reached
    if (++m_stage == 4)
    {
        if (m_ips[0] == m_ips[1] && m_ports[0] == m_ports[1])
        {
            m_type = 4;

            if (m_multiplePorts || m_ips[2] == m_ips[0] || m_ips[2] == m_ips[1])
                m_type = 1;
        }
        else if (m_ips[0] == m_ips[1])
            m_type = 2;
        else
            m_type = 0;

        static const char *typeNames[] = { "NAT Error", "Full Cone or Restricted Cone NAT", "Symmetric NAT", "Symmetric NAT", "Port-Restricted Cone NAT" };
        kDebug() << "NAT type: " << typeNames[m_type];

        // NAT check finished
        emit ready();
    }
}
