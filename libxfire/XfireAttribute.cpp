#include "XfireAttribute.h"

#include <QtEndian>

namespace Xfire
{
Attribute *Attribute::parseAttributeString ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the ID
    if ( ( p_len - p_offset ) < 4 )
        return 0;
    quint8 id_len = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    if ( ( p_len - p_offset ) < ( id_len + 2 ) )
        return 0;
    QString id = QString::fromUtf8 ( p_data.constData() + p_offset, id_len );
    p_offset += id_len;

    // Get the type
    quint8 type = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    switch ( type )
    {
    case String:
    {
        StringAttributeS *attr = new StringAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Int32:
    {
        Int32AttributeS *attr = new Int32AttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case SID:
    {
        SIDAttributeS *attr = new SIDAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case List:
    {
        ListAttributeS *attr = new ListAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case ParentString:
    {
        ParentStringAttributeS *attr = new ParentStringAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case CID:
    {
        CIDAttributeS *attr = new CIDAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Int64:
    {
        Int64AttributeS *attr = new Int64AttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Bool:
    {
        BoolAttributeS *attr = new BoolAttributeS ( id, false );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case ParentByte:
    {
        ParentByteAttributeS *attr = new ParentByteAttributeS ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    default:
        return 0;
    }
}

void StringIDAttribute::writeIDToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write length
    QByteArray id = m_id.toUtf8();
    id.truncate ( 255 );
    p_buffer.append ( id.length() );
    p_offset++;

    // Write string name
    p_buffer.append ( id );
    p_offset += id.length();

    // Write type
    p_buffer.append ( type() );
    p_offset++;
}

Attribute *Attribute::parseAttributeByte ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the ID
    if ( ( p_len - p_offset ) < 3 )
        return 0;
    quint8 id = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    // Get the type
    quint8 type = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    switch ( type )
    {
    case String:
    {
        StringAttributeB *attr = new StringAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Int32:
    {
        Int32AttributeB *attr = new Int32AttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case SID:
    {
        SIDAttributeB *attr = new SIDAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case List:
    {
        ListAttributeB *attr = new ListAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case ParentString:
    {
        ParentStringAttributeB *attr = new ParentStringAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case CID:
    {
        CIDAttributeB *attr = new CIDAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Int64:
    {
        Int64AttributeB *attr = new Int64AttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case Bool:
    {
        BoolAttributeB *attr = new BoolAttributeB ( id, false );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    case ParentByte:
    {
        ParentByteAttributeB *attr = new ParentByteAttributeB ( id );
        if ( !attr->parseData ( p_data, p_offset, p_len ) )
        {
            delete attr;
            return 0;
        }
        return attr;
    }
    default:
        return 0;
    }
}

void ByteIDAttribute::writeIDToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write ID
    p_buffer.append ( m_id );
    p_offset++;

    // Write type
    p_buffer.append ( type() );
    p_offset++;
}

bool StringAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the length
    if ( ( p_len - p_offset ) < 2 )
        return false;
    quint16 len = qFromLittleEndian ( *reinterpret_cast<const quint16*> ( p_data.constData() + p_offset ) );
    p_offset += 2;

    // Get the string
    if ( ( p_len - p_offset ) < len )
        return false;
    m_string = QString::fromUtf8 ( p_data.constData() + p_offset, len );
    p_offset += len;

    return true;
}

void StringAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    QByteArray string = m_string.toUtf8();
    string.truncate ( 0xFFFF );

    // Write the length
    quint16 len = qToLittleEndian ( static_cast<quint16> ( string.length() ) );
    p_buffer.append ( reinterpret_cast<const char*> ( &len ), 2 );
    p_offset += 2;

    // Write the string
    p_buffer.append ( string );
    p_offset += string.length();
}

bool Int32Attribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the integer
    if ( ( p_len - p_offset ) < 4 )
        return false;
    m_uint32 = qFromLittleEndian ( *reinterpret_cast<const quint32*> ( p_data.constData() + p_offset ) );
    p_offset += 4;

    return true;
}

void Int32Attribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write the integer
    quint32 integer = qToLittleEndian ( m_uint32 );
    p_buffer.append ( reinterpret_cast<const char*> ( &integer ), 4 );
    p_offset += 4;
}

bool SIDAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the SID
    if ( ( p_len - p_offset ) < 16 )
        return false;
    m_sid.setFromByteArray ( QByteArray ( p_data.constData() + p_offset, 16 ) );
    p_offset += 16;

    return true;
}

void SIDAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write the SID
    p_buffer.append ( m_sid.raw() );
    p_offset += 16;
}

ListAttribute::ListAttribute ( DataType p_dataType, const QList<ListElement> &p_elements ) :
        m_dataType ( p_dataType )
{
    foreach ( const ListElement &element, p_elements )
    {
        switch ( p_dataType )
        {
        case String:
            if ( element.string )
                m_elements.append ( element );
            break;
        case Int32:
            if ( element.int32 )
                m_elements.append ( element );
            break;
        case SID:
            if ( element.sid )
                m_elements.append ( element );
        case CID:
            if ( element.cid )
                m_elements.append ( element );
            break;
        case Int64:
            if ( element.int64 )
                m_elements.append ( element );
            break;
        case Bool:
            if ( element.boolean )
                m_elements.append ( element );
            break;
        }
    }
}

ListAttribute::~ListAttribute()
{
    foreach ( const ListElement &element, m_elements )
    {
        switch ( m_dataType )
        {
        case String:
            delete element.string;
            break;
        case Int32:
            delete element.int32;
            break;
        case SID:
            delete element.sid;
            break;
        case CID:
            delete element.cid;
            break;
        case Int64:
            delete element.int64;
            break;
        case Bool:
            delete element.boolean;
            break;
        }
    }
}

void ListAttribute::addElement ( const QString &p_string )
{
    if ( m_dataType == String )
    {
        ListElement element;
        element.string = new StringAttribute ( p_string );
        m_elements.append ( element );
    }
}

void ListAttribute::addElement ( quint32 p_uint32 )
{
    if ( m_dataType == Int32 )
    {
        ListElement element;
        element.int32 = new Int32Attribute ( p_uint32 );
        m_elements.append ( element );
    }
}

void ListAttribute::addElementSID ( const SessionID &p_sid )
{
    if ( m_dataType == SID )
    {
        ListElement element;
        element.sid = new SIDAttribute ( p_sid );
        m_elements.append ( element );
    }
}

void ListAttribute::addElementCID ( const ChatID &p_cid )
{
    if ( m_dataType == CID )
    {
        ListElement element;
        element.cid = new CIDAttribute ( p_cid );
        m_elements.append ( element );
    }
}

void ListAttribute::addElement ( quint64 p_uint64 )
{
    if ( m_dataType == Int64 )
    {
        ListElement element;
        element.int64 = new Int64Attribute ( p_uint64 );
        m_elements.append ( element );
    }
}

void ListAttribute::addElement ( quint8 p_byte )
{
    if ( m_dataType == Bool )
    {
        ListElement element;
        element.boolean = new BoolAttribute ( p_byte );
        m_elements.append ( element );
    }
}

void ListAttribute::addElement ( bool p_boolean )
{
    if ( m_dataType == Bool )
    {
        ListElement element;
        element.boolean = new BoolAttribute ( p_boolean );
        m_elements.append ( element );
    }
}

bool ListAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the type and len
    if ( ( p_len - p_offset ) < 3 )
        return false;
    m_dataType = static_cast<DataType> ( *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset ) );
    p_offset++;

    quint16 len = qFromLittleEndian ( *reinterpret_cast<const quint16*> ( p_data.constData() + p_offset ) );
    p_offset += 2;

    // Read the elements
    for ( quint16 i = 0; i < len; i++ )
    {
        ListElement element;
        switch ( m_dataType )
        {
        case String:
            element.string = new StringAttribute();
            if ( !element.string->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.string;
                return false;
            }
            break;
        case Int32:
            element.int32 = new Int32Attribute();
            if ( !element.int32->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.int32;
                return false;
            }
            break;
        case SID:
            element.sid = new SIDAttribute();
            if ( !element.sid->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.sid;
                return false;
            }
            break;
        case CID:
            element.cid = new CIDAttribute();
            if ( !element.cid->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.cid;
                return false;
            }
            break;
        case Int64:
            element.int64 = new Int64Attribute();
            if ( !element.int32->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.int64;
                return false;
            }
            break;
        case Bool:
            element.boolean = new BoolAttribute ( false );
            if ( !element.boolean->parseData ( p_data, p_offset, p_len ) )
            {
                delete element.boolean;
                return false;
            }
            break;
        }
        m_elements.append ( element );
    }

    return true;
}

void ListAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    p_buffer.append ( static_cast<quint8> ( m_dataType ) );
    p_offset++;

    quint16 len = qToLittleEndian ( static_cast<quint16> ( m_elements.length() ) );
    p_buffer.append ( reinterpret_cast<const char*> ( &len ), 2 );
    p_offset += 2;

    foreach ( const ListElement &element, m_elements )
    {
        switch ( m_dataType )
        {
        case String:
            element.string->writeDataToByteArray ( p_buffer, p_offset );
            break;
        case Int32:
            element.int32->writeDataToByteArray ( p_buffer, p_offset );
            break;
        case SID:
            element.sid->writeDataToByteArray ( p_buffer, p_offset );
            break;
        case CID:
            element.cid->writeDataToByteArray ( p_buffer, p_offset );
            break;
        case Int64:
            element.int64->writeDataToByteArray ( p_buffer, p_offset );
            break;
        case Bool:
            element.boolean->writeDataToByteArray ( p_buffer, p_offset );
            break;
        }
    }
}

ParentStringAttribute::~ParentStringAttribute()
{
    foreach ( StringIDAttribute *attr, m_attributes )
    delete attr;
}

const StringIDAttribute *ParentStringAttribute::getAttribute ( const QString &p_id ) const
{
    foreach ( const StringIDAttribute *attr, m_attributes )
    {
        if ( attr->id() == p_id )
            return attr;
    }

    return 0;
}

void ParentStringAttribute::addAttribute ( StringIDAttribute *p_attr )
{
    if ( p_attr )
    {
        if ( !p_attr->isValid() )
        {
            delete p_attr;
            return;
        }

        m_attributes.append ( p_attr );
    }
}

bool ParentStringAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get len
    if ( ( p_len - p_offset ) < 1 )
        return false;
    quint8 len = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    // Read attributes
    for ( quint8 i = 0; i < len; i++ )
    {
        StringIDAttribute *attr = reinterpret_cast<StringIDAttribute*> (
                                      Attribute::parseAttributeString ( p_data, p_offset, p_len ) );

        if ( !attr )
            return false;

        m_attributes.append ( attr );
    }

    return true;
}

void ParentStringAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write len
    p_buffer.append ( static_cast<quint8> ( m_attributes.length() ) );
    p_offset++;

    // Write attributes
    foreach ( const StringIDAttribute *attr, m_attributes )
    attr->writeToByteArray ( p_buffer, p_offset );
}

bool CIDAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the CID
    if ( ( p_len - p_offset ) < 21 )
        return false;
    m_cid.setFromByteArray ( QByteArray ( p_data.constData() + p_offset, 21 ) );
    p_offset += 21;

    return true;
}

void CIDAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write the SID
    p_buffer.append ( m_cid.raw() );
    p_offset += 21;
}

bool Int64Attribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the integer
    if ( ( p_len - p_offset ) < 8 )
        return false;
    m_uint64 = qFromLittleEndian ( *reinterpret_cast<const quint64*> ( p_data.constData() + p_offset ) );
    p_offset += 8;

    return true;
}

void Int64Attribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write the integer
    quint64 integer = qToLittleEndian ( m_uint64 );
    p_buffer.append ( reinterpret_cast<const char*> ( &integer ), 8 );
    p_offset += 8;
}

bool BoolAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get the byte
    if ( ( p_len - p_offset ) < 1 )
        return false;
    m_byte = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    return true;
}

void BoolAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write the byte
    p_buffer.append ( m_byte );
    p_offset++;
}

ParentByteAttribute::~ParentByteAttribute()
{
    foreach ( ByteIDAttribute *attr, m_attributes )
    delete attr;
}

const ByteIDAttribute *ParentByteAttribute::getAttribute ( quint8 p_id ) const
{
    foreach ( const ByteIDAttribute *attr, m_attributes )
    {
        if ( attr->id() == p_id )
            return attr;
    }

    return 0;
}

void ParentByteAttribute::addAttribute ( ByteIDAttribute *p_attr )
{
    if ( p_attr )
    {
        if ( !p_attr->isValid() )
        {
            delete p_attr;
            return;
        }

        m_attributes.append ( p_attr );
    }
}

bool ParentByteAttribute::parseData ( const QByteArray &p_data, quint32 &p_offset, quint32 p_len )
{
    // Get len
    if ( ( p_len - p_offset ) < 1 )
        return false;
    quint8 len = *reinterpret_cast<const quint8*> ( p_data.constData() + p_offset );
    p_offset++;

    // Read attributes
    for ( quint8 i = 0; i < len; i++ )
    {
        ByteIDAttribute *attr = reinterpret_cast<ByteIDAttribute*> (
                                    Attribute::parseAttributeByte ( p_data, p_offset, p_len ) );

        if ( !attr )
            return false;

        m_attributes.append ( attr );
    }

    return true;
}

void ParentByteAttribute::writeDataToByteArray ( QByteArray &p_buffer, quint32 &p_offset ) const
{
    // Write len
    p_buffer.append ( static_cast<quint8> ( m_attributes.length() ) );
    p_offset++;

    // Write attributes
    foreach ( const ByteIDAttribute *attr, m_attributes )
    attr->writeToByteArray ( p_buffer, p_offset );
}
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
