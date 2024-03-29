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

#ifndef XF_PROTOCOL_H
#define XF_PROTOCOL_H

#include <kopeteaccountmanager.h>
#include <kopeteproperty.h>

#include <kopeteglobal.h>
#include <kopeteonlinestatus.h>
#include <kopeteonlinestatusmanager.h>
#include <kopeteproperty.h>
#include <kopeteprotocol.h>

#define XFIRE_SERVER_IP "cs.xfire.com"
#define XFIRE_SERVER_PORT 25999
#define XFIRE_PROTO_VERSION 132

namespace Kopete
{
class Contact;
class MetaContact;
}

class XfireProtocol : public Kopete::Protocol
{
    Q_OBJECT

public:
    XfireProtocol(QObject *parent, const QVariantList &args);
    ~XfireProtocol();

    // Creates new protocol account
    virtual Kopete::Account *createNewAccount(const QString &accountID);
    virtual Kopete::Contact *deserializeContact(Kopete::MetaContact *metaContact, const QMap<QString, QString> &serializedData, const QMap<QString, QString> &);

    // Creates add contact & edit account widget
    virtual AddContactPage *createAddContactWidget(QWidget *parent, Kopete::Account *account);
    virtual KopeteEditAccountWidget *createEditAccountWidget(Kopete::Account *account, QWidget *parent);

    // Statuses
    const Kopete::OnlineStatus XfireOffline;
    const Kopete::OnlineStatus XfireConnecting;
    const Kopete::OnlineStatus XfireOnline;
    const Kopete::OnlineStatus XfireAway;

    // Properties
    const Kopete::PropertyTmpl propGame;
    const Kopete::PropertyTmpl propServer;

    static XfireProtocol *protocol();

private:
    static XfireProtocol *m_protocol; // Protocol instance
};

#endif // XF_PROTOCOL_H
