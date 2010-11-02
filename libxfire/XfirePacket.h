#ifndef XFIREPACKET_H
#define XFIREPACKET_H

#include "XfireAttribute.h"

#include <QList>

#define XFIRE_HEADER_LEN 5

namespace Xfire
{
	class Packet
	{
	public:
		Packet(quint16 p_id);
		~Packet();

		bool isValid() const;
		quint16 id() const;
		quint8 numAttributes() const;

		void addAttribute(Attribute *p_attribute);

		const StringIDAttribute *getAttribute(const QString &p_id) const;
		const ByteIDAttribute *getAttribute(quint8 p_id) const;

		QByteArray toByteArray() const;

		static quint16 requiredLen(const QByteArray &p_data);
		static Packet *parseData(const QByteArray &p_data);

	private:
		quint16 m_id;
		QList<Attribute*> m_attributes;
	};
}

#endif // XFIREPACKET_H
