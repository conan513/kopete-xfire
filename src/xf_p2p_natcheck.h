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

#ifndef XF_P2P_NATCHECK_H
#define XF_P2P_NATCHECK_H

#define XFIRE_NAT_PORT 9856

#include <QUdpSocket>

class XfireP2P;

class XfireP2PNatcheck : public QObject
{
    Q_OBJECT;

public:
    // Constructor & destructor
    XfireP2PNatcheck(XfireP2P *p_p2p);
    ~XfireP2PNatcheck();

    XfireP2P *m_p2p;
    QUdpSocket *m_connection;

    bool m_multiplePorts;
    int m_stage;
    int m_type;

    QString m_servers[3];
    quint32 m_ips[3];
    quint16 m_ports[3];

private slots:
    void slotSocketRead();

signals:
    void ready();
};

#endif // XF_P2P_NATCHECK_H
