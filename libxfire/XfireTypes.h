#ifndef XFIRETYPES_H
#define XFIRETYPES_H

#include <QByteArray>

namespace Xfire
{
class SessionID
{
public:
    SessionID ( const QByteArray &p_sessionID = QByteArray() );
    SessionID ( const SessionID &p_sessionID );

    bool isValid() const;

    inline const QByteArray &raw() const { return m_sessionID; }

    void setFromByteArray ( const QByteArray &p_sessionID );

    bool operator== ( const SessionID &p_sessionID ) const;
    SessionID &operator= ( const SessionID &p_sessionID );

private:
    QByteArray m_sessionID;
};

class ChatID
{
public:
    ChatID ( const QByteArray &p_chatID = QByteArray() );
    ChatID ( const ChatID &p_chatID );

    bool isValid() const;

    inline const QByteArray &raw() const { return m_chatID; }

    void setFromByteArray ( const QByteArray &p_sessionID );

    bool operator== ( const ChatID &p_chatID ) const;
    ChatID &operator= ( const ChatID &p_chatID );

private:
    QByteArray m_chatID;
};
}

#endif // XFIRETYPES_H
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
