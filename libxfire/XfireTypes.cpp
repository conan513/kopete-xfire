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

#include "XfireTypes.h"

namespace Xfire
{
SessionID::SessionID (const QByteArray &p_sessionID)
{
    setFromByteArray (p_sessionID);
}

SessionID::SessionID (const SessionID &p_sessionID)
{
    operator= (p_sessionID);
}

bool SessionID::isValid() const
{
    static const unsigned char invalid[] =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

    return (memcmp (invalid, m_sessionID.constData(), 16) != 0);
}

void SessionID::setFromByteArray (const QByteArray &p_sessionID)
{
    if (p_sessionID.length() != 16)
        m_sessionID.fill (0, 16);
    else
        m_sessionID = p_sessionID;
}

bool SessionID::operator== (const SessionID &p_sessionID) const
{
    return (memcmp (p_sessionID.m_sessionID.constData(), m_sessionID.constData(), 16) == 0);
}

SessionID &SessionID::operator= (const SessionID &p_sessionID)
{
    m_sessionID = p_sessionID.m_sessionID;
    return *this;
}

ChatID::ChatID (const QByteArray &p_chatID)
{
    setFromByteArray (p_chatID);
}

ChatID::ChatID (const ChatID &p_chatID)
{
    operator= (p_chatID);
}

bool ChatID::isValid() const
{
    static const unsigned char invalid[] =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00
        };

    return (memcmp (invalid, m_chatID.constData(), 21) != 0);
}

void ChatID::setFromByteArray (const QByteArray &p_chatID)
{
    if (p_chatID.length() != 21)
        m_chatID.fill (0, 21);
    else
        m_chatID = p_chatID;
}

bool ChatID::operator== (const ChatID &p_chatID) const
{
    return (memcmp (p_chatID.m_chatID.constData(), m_chatID.constData(), 21) == 0);
}

ChatID &ChatID::operator= (const ChatID &p_chatID)
{
    m_chatID = p_chatID.m_chatID;
    return *this;
}
}
