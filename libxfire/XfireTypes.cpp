#include "XfireTypes.h"

namespace Xfire
{
SessionID::SessionID ( const QByteArray &p_sessionID )
{
    setFromByteArray ( p_sessionID );
}

SessionID::SessionID ( const SessionID &p_sessionID )
{
    operator= ( p_sessionID );
}

bool SessionID::isValid() const
{
    static const unsigned char invalid[] =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

    return ( memcmp ( invalid, m_sessionID.constData(), 16 ) != 0 );
}

void SessionID::setFromByteArray ( const QByteArray &p_sessionID )
{
    if ( p_sessionID.length() != 16 )
        m_sessionID.fill ( 0, 16 );
    else
        m_sessionID = p_sessionID;
}

bool SessionID::operator== ( const SessionID &p_sessionID ) const
{
    return ( memcmp ( p_sessionID.m_sessionID.constData(), m_sessionID.constData(), 16 ) == 0 );
}

SessionID &SessionID::operator= ( const SessionID &p_sessionID )
{
    m_sessionID = p_sessionID.m_sessionID;
    return *this;
}

ChatID::ChatID ( const QByteArray &p_chatID )
{
    setFromByteArray ( p_chatID );
}

ChatID::ChatID ( const ChatID &p_chatID )
{
    operator= ( p_chatID );
}

bool ChatID::isValid() const
{
    static const unsigned char invalid[] =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00
        };

    return ( memcmp ( invalid, m_chatID.constData(), 21 ) != 0 );
}

void ChatID::setFromByteArray ( const QByteArray &p_chatID )
{
    if ( p_chatID.length() != 21 )
        m_chatID.fill ( 0, 21 );
    else
        m_chatID = p_chatID;
}

bool ChatID::operator== ( const ChatID &p_chatID ) const
{
    return ( memcmp ( p_chatID.m_chatID.constData(), m_chatID.constData(), 21 ) == 0 );
}

ChatID &ChatID::operator= ( const ChatID &p_chatID )
{
    m_chatID = p_chatID.m_chatID;
    return *this;
}
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
