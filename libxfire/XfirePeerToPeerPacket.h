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
