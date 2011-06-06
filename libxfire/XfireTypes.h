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

#ifndef XFIRETYPES_H
#define XFIRETYPES_H

#include <QByteArray>

namespace Xfire
{
class SessionID
{
public:
    SessionID(const QByteArray &p_sessionID = QByteArray());
    SessionID(const SessionID &p_sessionID);

    bool isValid() const;

    inline const QByteArray &raw() const { return m_sessionID; }

    void setFromByteArray(const QByteArray &p_sessionID);

    bool operator==(const SessionID &p_sessionID) const;
    SessionID &operator=(const SessionID &p_sessionID);

private:
    QByteArray m_sessionID;
};

class ChatID
{
public:
    ChatID(const QByteArray &p_chatID = QByteArray());
    ChatID(const ChatID &p_chatID);

    bool isValid() const;

    inline const QByteArray &raw() const { return m_chatID; }

    void setFromByteArray(const QByteArray &p_sessionID);

    bool operator==(const ChatID &p_chatID) const;
    ChatID &operator=(const ChatID &p_chatID);

private:
    QByteArray m_chatID;
};
}

#endif // XFIRETYPES_H
