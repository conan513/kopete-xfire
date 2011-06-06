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

#include "XfirePeerToPeerPacket.h"

#include <QtEndian>

#include <KDebug>

namespace Xfire
{
PeerToPeerPacket::PeerToPeerPacket(quint16 p_id) : m_id(p_id)
{
}

PeerToPeerPacket::~PeerToPeerPacket()
{
    foreach(Attribute *attr, m_attributes)
        delete attr;
}

bool PeerToPeerPacket::isValid() const
{
    return(m_id != 0);
}

quint16 PeerToPeerPacket::id() const
{
    return m_id;
}

quint8 PeerToPeerPacket::numAttributes() const
{
    return m_attributes.length();
}

void PeerToPeerPacket::addAttribute(Attribute *p_attribute)
{
    if(!p_attribute->isValid())
    {
        delete p_attribute;
        return;
    }

    m_attributes.append(p_attribute);
}

const StringIDAttribute *PeerToPeerPacket::getAttribute(const QString &p_id) const
{
    foreach(const Attribute *attr, m_attributes)
    {
        if(attr->isValid() &&(attr->idType() == Attribute::StringID) &&
               (static_cast<const StringIDAttribute*>(attr)->id() == p_id))
            
            return static_cast<const StringIDAttribute*>(attr);
    }

    return 0;
}

const ByteIDAttribute *PeerToPeerPacket::getAttribute(quint8 p_id) const
{
    foreach(const Attribute *attr, m_attributes)
    {
        if(attr->isValid() &&(attr->idType() == Attribute::ByteID) &&
               (static_cast<const ByteIDAttribute*>(attr)->id() == p_id))
            
            return static_cast<const ByteIDAttribute*>(attr);
    }

    return 0;
}

QByteArray PeerToPeerPacket::toByteArray() const
{
    QByteArray buffer;
    
    if(!isValid())
        return buffer;

    buffer.fill(0, 4); // Empty len

    // ID
    quint16 id = qToLittleEndian(m_id);
    buffer.append(reinterpret_cast<const char*>(&id), 2);

    buffer.append(m_attributes.length()); // Attribute count

    // Attributes
    quint32 offset = XFIRE_HEADER_LEN_P2P;
    foreach(const Attribute *attr, m_attributes)
        attr->writeToByteArray(buffer, offset);

    // Len
    quint32 len = qToLittleEndian(static_cast<quint32>(offset));
    memcpy(buffer.data(), &len, 4);

    return buffer;
}

quint32 PeerToPeerPacket::requiredLen(const QByteArray &p_data)
{
    // Return the minimum length (to have the actual length)
    if(p_data.length() < 4)
        return 4;

    // Return the length as the header states (first two bytes)
    return qFromLittleEndian(*(reinterpret_cast<const quint32*>(p_data.constData())));
}

PeerToPeerPacket *PeerToPeerPacket::parseData(const QByteArray &p_data)
{
    // Return an invalid packet, if there is not even a complete header
    if(p_data.length() < XFIRE_HEADER_LEN_P2P)
        return 0;

    quint32 offset = 0;

    // Packet length
    quint32 len = requiredLen(p_data);
    offset += 4;
    
    // Return an invalid packet, if there is not enough data available
    if((len < XFIRE_HEADER_LEN_P2P) || (p_data.length() < len))
        return 0;
    
    // Packet id
    quint16 id = qFromLittleEndian(*(reinterpret_cast<const quint16*>(p_data.constData() + offset)));
    offset += 2;

    // Attribute count
    quint8 attr_count = *(reinterpret_cast<const quint8*>(p_data.constData() + offset));
    offset += 1;

    PeerToPeerPacket *ret = new PeerToPeerPacket(id);

    static const quint16 stringPackets[] =
    {
        0x3E87, 0x3E88, 0x3E89, 0x3E8A, 0x3E8B, 0x3E8C, 0x3E8D, 0x3E8E, 0x0000
    };

    bool uses_strings = false;
    int i = 0;
    
    while(stringPackets[i] != 0x0000)
    {
        if(stringPackets[i] == id)
        {
            uses_strings = true;
            break;
        }

        i++;
    }

    for(i = 0; i < attr_count; i++)
    {
        Attribute *attr = uses_strings ? Attribute::parseAttributeString(p_data, offset, len) : Attribute::parseAttributeByte(p_data, offset, len);
        // Return an invalid packet, if at least one attribute parsing failed
        if(!attr || !attr->isValid())
        {
            if(attr)
                delete attr;

            delete ret;
            return 0;
        }

        ret->m_attributes.append(attr);
    }

    // Return an invalid packet, if the size did not match the attributes
    if(offset != len)
    {
        delete ret;
        return 0;
    }

    return ret;
}
}
