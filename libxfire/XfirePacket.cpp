#include "XfirePacket.h"

#include <QtEndian>

namespace Xfire
{
	Packet::Packet(quint16 p_id) :
			m_id(p_id)
	{
	}

	Packet::~Packet()
	{
		foreach(Attribute *attr, m_attributes)
			delete attr;
	}

	bool Packet::isValid() const
	{
		return (m_id != 0);
	}

	quint16 Packet::id() const
	{
		return m_id;
	}

	quint8 Packet::numAttributes() const
	{
		return m_attributes.length();
	}

	void Packet::addAttribute(Attribute *p_attribute)
	{
		if(!p_attribute->isValid())
		{
			delete p_attribute;
			return;
		}

		m_attributes.append(p_attribute);
	}

	const StringIDAttribute *Packet::getAttribute(const QString &p_id) const
	{
		foreach(const Attribute *attr, m_attributes)
		{
			if(attr->isValid() && (attr->idType() == Attribute::StringID) &&
			   (static_cast<const StringIDAttribute*>(attr)->id() == p_id))
				return static_cast<const StringIDAttribute*>(attr);
		}

		return 0;
	}

	const ByteIDAttribute *Packet::getAttribute(quint8 p_id) const
	{
		foreach(const Attribute *attr, m_attributes)
		{
			if(attr->isValid() && (attr->idType() == Attribute::ByteID) &&
			   (static_cast<const ByteIDAttribute*>(attr)->id() == p_id))
				return static_cast<const ByteIDAttribute*>(attr);
		}

		return 0;
	}

	QByteArray Packet::toByteArray() const
	{
		QByteArray buffer;
		if(!isValid())
			return buffer;

		// Empty len
		buffer.fill(0, 2);

		// ID
		quint16 id = qToLittleEndian(m_id);
		buffer.append(reinterpret_cast<const char*>(&id), 2);

		// Attribute count
		buffer.append(m_attributes.length());

		// Attributes
		quint32 offset = XFIRE_HEADER_LEN;
		foreach(const Attribute *attr, m_attributes)
			attr->writeToByteArray(buffer, offset);

		// Len
		quint16 len = qToLittleEndian(static_cast<quint16>(offset));
		memcpy(buffer.data(), &len, 2);

		return buffer;
	}

	quint16 Packet::requiredLen(const QByteArray &p_data)
	{
		// Return the minimum length (to have the actual length)
		if(p_data.length() < 2)
			return 2;

		// Return the length as the header states (first two bytes)
		return qFromLittleEndian(*(reinterpret_cast<const quint16*>(p_data.constData())));
	}

	Packet *Packet::parseData(const QByteArray &p_data)
	{
		// Return an invalid packet, if there is not even a complete header
		if(p_data.length() < XFIRE_HEADER_LEN)
			return 0;

		quint32 offset = 0;

		// Packet length
		quint16 len = requiredLen(p_data);
		offset += 2;

		// Return an invalid packet, if there is not enough data available
		if((len < XFIRE_HEADER_LEN) || (p_data.length() < len))
			return 0;

		// Packet id
		quint16 id = qFromLittleEndian(*(reinterpret_cast<const quint16*>(p_data.constData() + offset)));
		offset += 2;

		// Attribute count
		quint8 attr_count = *(reinterpret_cast<const quint8*>(p_data.constData() + offset));
		offset += 1;

		Packet *ret = new Packet(id);

		static const quint16 stringPackets[] =
		{
			0x0002, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A,
			0x008B, 0x008F, 0x0090, 0x0091, 0x0093, 0x0094, 0x0095, 0x009A, 0x009C, 0x0190, 0x0191,
			0x0192, 0x0194, 0x0000
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
			Attribute *attr = uses_strings ? Attribute::parseAttributeString(p_data, offset, len) :
							 Attribute::parseAttributeByte(p_data, offset, len);
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
