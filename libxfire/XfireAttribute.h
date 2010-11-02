#ifndef XFIREATTRIBUTE_H
#define XFIREATTRIBUTE_H

#include "XfireTypes.h"

#include <QString>
#include <QByteArray>
#include <QList>
#include <QDebug>

#define WRITE_TO_BYTE_ARRAY \
	virtual void writeToByteArray(QByteArray &p_buffer, quint32 &p_offset) const \
	{ \
		writeIDToByteArray(p_buffer, p_offset); \
		writeDataToByteArray(p_buffer, p_offset); \
	}

namespace Xfire
{
	class Attribute
	{
	public:
		enum IDType
		{
			StringID,
			ByteID
		};

		enum AttributeType
		{
			String = 0x01,
			Int32 = 0x02,
			SID = 0x03,
			List = 0x04,
			ParentString = 0x05,
			CID = 0x06,
			Int64 = 0x07,
			Bool = 0x08,
			ParentByte = 0x09
		};

		virtual ~Attribute() {}

		bool isValid() const { return m_valid; }
		IDType idType() const { return m_idType; }
		AttributeType type() const { return m_attrType; }

		virtual void writeToByteArray(QByteArray &, quint32 &) const = 0;

		static Attribute *parseAttributeString(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		static Attribute *parseAttributeByte(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);

	protected:
		Attribute(IDType p_idType, AttributeType p_attrType) :
				m_valid(true), m_idType(p_idType), m_attrType(p_attrType) {}

		bool m_valid;

	private:
		IDType m_idType;
		AttributeType m_attrType;
	};

	class StringIDAttribute : public Attribute
	{
	public:
		const QString &id() const { return m_id; }

	protected:
		StringIDAttribute(const QString &p_id, AttributeType p_type) :
				Attribute(StringID, p_type), m_id(p_id) {}

		void writeIDToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		QString m_id;
	};

	class ByteIDAttribute : public Attribute
	{
	public:
		quint8 id() const { return m_id; }

	protected:
		ByteIDAttribute(quint8 p_id, AttributeType p_type) :
				Attribute(ByteID, p_type), m_id(p_id) {}

		void writeIDToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		quint8 m_id;
	};

	class StringAttribute
	{
	public:
		StringAttribute(const QString &p_string = QString()) :
				m_string(p_string) {}

		const QString &string() const { return m_string; }
		quint16 length() const { return m_string.length(); }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		QString m_string;
	};

	class StringAttributeS : public StringIDAttribute, public StringAttribute
	{
	public:
		StringAttributeS(const QString &p_id, const QString &p_string = QString()) :
				StringIDAttribute(p_id, String), StringAttribute(p_string) {}

		WRITE_TO_BYTE_ARRAY
	};

	class StringAttributeB : public ByteIDAttribute, public StringAttribute
	{
	public:
		StringAttributeB(quint8 p_id, const QString &p_string = QString()) :
				ByteIDAttribute(p_id, String), StringAttribute(p_string) {}

		WRITE_TO_BYTE_ARRAY
	};

	class Int32Attribute
	{
	public:
		Int32Attribute(quint32 p_uint32 = 0) :
				m_uint32(p_uint32) {}

		quint32 value() const { return m_uint32; }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		quint32 m_uint32;
	};

	class Int32AttributeS : public StringIDAttribute, public Int32Attribute
	{
	public:
		Int32AttributeS(const QString &p_id, quint32 p_uint32 = 0) :
				StringIDAttribute(p_id, Int32), Int32Attribute(p_uint32) {}

		WRITE_TO_BYTE_ARRAY
	};

	class Int32AttributeB : public ByteIDAttribute, public Int32Attribute
	{
	public:
		Int32AttributeB(quint8 p_id, quint32 p_uint32 = 0) :
				ByteIDAttribute(p_id, Int32), Int32Attribute(p_uint32) {}

		WRITE_TO_BYTE_ARRAY
	};

	class SIDAttribute
	{
	public:
		SIDAttribute(const SessionID &p_sid = SessionID()) :
				m_sid(p_sid) {}

		const SessionID &sid() const { return m_sid; }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		SessionID m_sid;
	};

	class SIDAttributeS : public StringIDAttribute, public SIDAttribute
	{
	public:
		SIDAttributeS(const QString &p_id, const SessionID &p_sid = SessionID()) :
				StringIDAttribute(p_id, SID), SIDAttribute(p_sid) {}

		WRITE_TO_BYTE_ARRAY
	};

	class SIDAttributeB : public ByteIDAttribute, public SIDAttribute
	{
	public:
		SIDAttributeB(quint8 p_id, const SessionID &p_sid = SessionID()) :
				ByteIDAttribute(p_id, SID), SIDAttribute(p_sid) {}

		WRITE_TO_BYTE_ARRAY
	};

	// Forward declarations for the list
	class CIDAttribute;
	class Int64Attribute;
	class BoolAttribute;

	class ListAttribute
	{
	public:
		enum DataType
		{
			String = 0x01,
			Int32 = 0x02,
			SID = 0x03,
			CID = 0x06,
			Int64 = 0x07,
			Bool = 0x08
		};

		union ListElement
		{
			StringAttribute *string;
			Int32Attribute *int32;
			SIDAttribute *sid;
			CIDAttribute *cid;
			Int64Attribute *int64;
			BoolAttribute *boolean;
		};

		ListAttribute(DataType p_dataType = Int32, const QList<ListElement> &p_elements = QList<ListElement>());
		~ListAttribute();

		DataType dataType() const { return m_dataType; }
		quint16 numElements() const { return m_elements.length(); }
		const QList<ListElement> &elements() const { return m_elements; }

		void addElement(const QString &p_string);
		void addElement(quint32 p_uint32);
		void addElementSID(const SessionID &p_sid);
		void addElementCID(const ChatID &p_cid);
		void addElement(quint64 p_uint64);
		void addElement(quint8 p_byte);
		void addElement(bool p_boolean);

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		DataType m_dataType;
		QList<ListElement> m_elements;
	};

	class ListAttributeS : public StringIDAttribute, public ListAttribute
	{
	public:
		ListAttributeS(const QString &p_id, DataType p_dataType = ListAttribute::Int32,
					   const QList<ListElement> &p_elements = QList<ListElement>()) :
				StringIDAttribute(p_id, List), ListAttribute(p_dataType, p_elements) {}

		WRITE_TO_BYTE_ARRAY
	};

	class ListAttributeB : public ByteIDAttribute, public ListAttribute
	{
	public:
		ListAttributeB(quint8 p_id, DataType p_dataType = ListAttribute::Int32,
					   const QList<ListElement> &p_elements = QList<ListElement>()) :
				ByteIDAttribute(p_id, List), ListAttribute(p_dataType, p_elements) {}

		WRITE_TO_BYTE_ARRAY
	};

	class ParentStringAttribute
	{
	public:
		ParentStringAttribute(const QList<StringIDAttribute*> &p_attributes = QList<StringIDAttribute*>()) :
				m_attributes(p_attributes) {}
		~ParentStringAttribute();

		quint8 numAttributes() const { return m_attributes.length(); }
		const StringIDAttribute *getAttribute(const QString &p_id) const;

		void addAttribute(StringIDAttribute *p_attr);

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		QList<StringIDAttribute*> m_attributes;
	};

	class ParentStringAttributeS : public StringIDAttribute, public ParentStringAttribute
	{
	public:
		ParentStringAttributeS(const QString &p_id,
							   const QList<StringIDAttribute*> &p_attributes = QList<StringIDAttribute*>()) :
				StringIDAttribute(p_id, ParentString), ParentStringAttribute(p_attributes) {}

		WRITE_TO_BYTE_ARRAY
	};

	class ParentStringAttributeB : public ByteIDAttribute, public ParentStringAttribute
	{
	public:
		ParentStringAttributeB(quint8 p_id, const QList<StringIDAttribute*> &p_attributes = QList<StringIDAttribute*>()) :
				ByteIDAttribute(p_id, ParentString), ParentStringAttribute(p_attributes) {}

		WRITE_TO_BYTE_ARRAY
	};

	class CIDAttribute
	{
	public:
		CIDAttribute(const ChatID &p_cid = ChatID()) :
				m_cid(p_cid) {}

		const ChatID &cid() const { return m_cid; }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		ChatID m_cid;
	};

	class CIDAttributeS : public StringIDAttribute, public CIDAttribute
	{
	public:
		CIDAttributeS(const QString &p_id, const ChatID &p_cid = ChatID()) :
				StringIDAttribute(p_id, CID), CIDAttribute(p_cid) {}

		WRITE_TO_BYTE_ARRAY
	};

	class CIDAttributeB : public ByteIDAttribute, public CIDAttribute
	{
	public:
		CIDAttributeB(quint8 p_id, const ChatID &p_cid = ChatID()) :
				ByteIDAttribute(p_id, CID), CIDAttribute(p_cid) {}

		WRITE_TO_BYTE_ARRAY
	};

	class Int64Attribute
	{
	public:
		Int64Attribute(quint64 p_uint64 = 0) :
				m_uint64(p_uint64) {}

		quint64 value() const { return m_uint64; }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		quint64 m_uint64;
	};

	class Int64AttributeS : public StringIDAttribute, public Int64Attribute
	{
	public:
		Int64AttributeS(const QString &p_id, quint64 p_uint64 = 0) :
				StringIDAttribute(p_id, Int64), Int64Attribute(p_uint64) {}

		WRITE_TO_BYTE_ARRAY
	};

	class Int64AttributeB : public ByteIDAttribute, public Int64Attribute
	{
	public:
		Int64AttributeB(quint8 p_id, quint32 p_uint64 = 0) :
				ByteIDAttribute(p_id, Int64), Int64Attribute(p_uint64) {}

		WRITE_TO_BYTE_ARRAY
	};

	class BoolAttribute
	{
	public:
		BoolAttribute(quint8 p_raw) :
				m_byte(p_raw) {}
		BoolAttribute(bool p_isSet) { p_isSet ? m_byte = 1 : m_byte = 0; }

		quint8 raw() const { return m_byte; }
		bool isSet() const { return (m_byte != 0); }

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		quint8 m_byte;
	};

	class BoolAttributeS : public StringIDAttribute, public BoolAttribute
	{
	public:
		BoolAttributeS(const QString &p_id, bool p_isSet) :
				StringIDAttribute(p_id, Bool), BoolAttribute(p_isSet) {}
		BoolAttributeS(const QString &p_id, quint8 p_raw) :
				StringIDAttribute(p_id, Bool), BoolAttribute(p_raw) {}

		WRITE_TO_BYTE_ARRAY
	};

	class BoolAttributeB : public ByteIDAttribute, public BoolAttribute
	{
	public:
		BoolAttributeB(quint8 p_id, bool p_isSet) :
				ByteIDAttribute(p_id, Bool), BoolAttribute(p_isSet) {}
		BoolAttributeB(quint8 p_id, quint8 p_raw) :
				ByteIDAttribute(p_id, Bool), BoolAttribute(p_raw) {}

		WRITE_TO_BYTE_ARRAY
	};

	class ParentByteAttribute
	{
	public:
		ParentByteAttribute(const QList<ByteIDAttribute*> &p_attributes = QList<ByteIDAttribute*>()) :
				m_attributes(p_attributes) {}
		~ParentByteAttribute();

		quint8 numAttributes() const { return m_attributes.length(); }
		const ByteIDAttribute *getAttribute(quint8 p_id) const;

		void addAttribute(ByteIDAttribute *p_attr);

		bool parseData(const QByteArray &p_data, quint32 &p_offset, quint32 p_len);
		void writeDataToByteArray(QByteArray &p_buffer, quint32 &p_offset) const;

	private:
		QList<ByteIDAttribute*> m_attributes;
	};

	class ParentByteAttributeS : public StringIDAttribute, public ParentByteAttribute
	{
	public:
		ParentByteAttributeS(const QString &p_id,
							 const QList<ByteIDAttribute*> &p_attributes = QList<ByteIDAttribute*>()) :
				StringIDAttribute(p_id, ParentByte), ParentByteAttribute(p_attributes) {}

		WRITE_TO_BYTE_ARRAY
	};

	class ParentByteAttributeB : public ByteIDAttribute, public ParentByteAttribute
	{
	public:
		ParentByteAttributeB(quint8 p_id, const QList<ByteIDAttribute*> &p_attributes = QList<ByteIDAttribute*>()) :
				ByteIDAttribute(p_id, ParentByte), ParentByteAttribute(p_attributes) {}

		WRITE_TO_BYTE_ARRAY
	};
}

#endif // XFIREATTRIBUTE_H
