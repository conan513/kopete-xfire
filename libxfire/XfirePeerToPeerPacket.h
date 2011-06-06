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

#ifndef XFIREPEERTOPEERPACKET_H
#define XFIREPEERTOPEERPACKET_H

#include "XfireAttribute.h"

#include <QList>

#define XFIRE_HEADER_LEN_P2P 7

namespace Xfire
{
class PeerToPeerPacket
{
public:
    PeerToPeerPacket(quint16 p_id);
    ~PeerToPeerPacket();

    bool isValid() const;
    quint16 id() const;
    quint8 numAttributes() const;

    void addAttribute(Attribute *p_attribute);

    const StringIDAttribute *getAttribute(const QString &p_id) const;
    const ByteIDAttribute *getAttribute(quint8 p_id) const;

    QByteArray toByteArray() const;

    static quint32 requiredLen(const QByteArray &p_data);
    static PeerToPeerPacket *parseData(const QByteArray &p_data);

private:
    quint16 m_id;
    QList<Attribute*> m_attributes;
};
}

#endif // XFIREPACKET_H
