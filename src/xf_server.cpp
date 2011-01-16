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

#include <KDebug>

#include <kopeteaddedinfoevent.h>
#include <kopetecontactlist.h>
#include <kopetegroup.h>
#include <kopetepassword.h>

#include "xf_contact.h"
#include "xf_protocol.h"
#include "xf_p2p_natcheck.h"
#include "xf_server.h"

XfireServer::XfireServer(XfireAccount *parent) : QObject(parent), m_account(parent)
{
    // Create socket to the Xfire server
    m_connection = new QTcpSocket(this);

    // Heartbeat & heartbeat timeout
    m_heartBeat = new QTimer(this);
    m_connectionTimeout = new QTimer(this);

    kDebug() << "Waiting for a connection to the Xfire server";

    // Signals
    connect(m_connection, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_connection, SIGNAL(readyRead()), this, SLOT(socketRead()));
    connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotConnectionInterrupted(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(goOnline()), m_account, SLOT(slotGoOnline()));
    connect(this, SIGNAL(goOffline()), m_account, SLOT(slotGoOffline()));

    connect(this, SIGNAL(ourStatusChanged(const Kopete::OnlineStatus &)), m_account, SLOT(changeOurStatus(const Kopete::OnlineStatus &)));

    connect(m_heartBeat, SIGNAL(timeout()), this, SLOT(slotSendHeartBeat()));
    connect(m_connectionTimeout, SIGNAL(timeout()), this, SLOT(slotConnectionInterrupted()));
}

XfireServer::~XfireServer()
{
}

void XfireServer::connectToServer(const QString accountId, const QString accountPass, const QString serverName, const uint serverPort)
{
    // Set account & password
    m_username = accountId;
    m_password = accountPass;

    m_connection->connectToHost(serverName, serverPort); // Connect to server
}

void XfireServer::slotConnected()
{
    // Emit connection error if we could not connect to the server
    if(!m_connection->waitForConnected())
    {
        // Unknown connection error
        kDebug() << "An error occured while connecting to the Xfire server";
        m_account->logOff(Kopete::Account::Unknown);
        return;
    }
    else
        kDebug() << "Connection to the Xfire server established";

    // Init connection
    m_connection->write("UA01");

    Xfire::Packet version(0x03);
    version.addAttribute(new Xfire::Int32AttributeS("version", m_account->protocolVersion()));
    m_connection->write(version.toByteArray());
}

void XfireServer::login(const QString &p_salt)
{
    kDebug() << "Sending login information";

    QCryptographicHash hasher(QCryptographicHash::Sha1);

    // Subhash sha1(username+password+UltimateArena)
    QByteArray subhash;
    hasher.addData(m_username.toUtf8());
    hasher.addData(m_password.toUtf8());
    hasher.addData("UltimateArena");
    subhash = hasher.result();

    // Hash sha1(subhash+salt)
    hasher.reset();
    hasher.addData(subhash.toHex());
    hasher.addData(p_salt.toAscii());
    QByteArray hash = hasher.result();

    Xfire::Packet login(0x0001);
    login.addAttribute(new Xfire::StringAttributeS("name", m_username));
    login.addAttribute(new Xfire::StringAttributeS("password", hash.toHex()));
    login.addAttribute(new Xfire::Int32AttributeS("flags", 0));

    m_connection->write(login.toByteArray());
}

void XfireServer::closeConnection()
{
    m_connection->close();

    // Remove unneeded timers
    m_heartBeat->stop();
    m_connectionTimeout->stop();
}

void XfireServer::socketRead()
{
    m_buffer.append(m_connection->readAll());

    // Parse all available packets
    quint16 len = 0;
    while (( len = Xfire::Packet::requiredLen(m_buffer)) <= m_buffer.length())
    {
        Xfire::Packet *packet = Xfire::Packet::parseData(m_buffer);
        if(!packet || !packet->isValid())
        {
            kDebug() << "Invalid packet received";

            if(packet)
                delete packet;

            m_account->logOff(Kopete::Account::Unknown);
            return;
        }

        handlePacket(packet);
        delete packet;

        m_buffer.remove(0, len);
    }
}

void XfireServer::sendAskFriendExtendedInformation(quint32 p_uid)
{
    Xfire::Packet foo(0x0025);
    foo.addAttribute(new Xfire::Int32AttributeB(0x0001, p_uid));

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendChat(const Xfire::SessionID &p_sid, quint32 p_chatMessageIndex, const QString &p_message)
{
    Xfire::Packet foo(0x0002);
    foo.addAttribute(new Xfire::SIDAttributeS("sid", p_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 0));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    peermsg->addAttribute(new Xfire::StringAttributeS("im", p_message));
    foo.addAttribute(peermsg);

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendChatConfirmation(const Xfire::SessionID &p_sid, quint32 p_chatMessageIndex)
{
    Xfire::Packet foo(0x0002);
    foo.addAttribute(new Xfire::SIDAttributeS("sid", p_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 1));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    foo.addAttribute(peermsg);

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendNickname(const QString &p_nickname)
{
    Xfire::Packet foo(0x000e);
    foo.addAttribute(new Xfire::StringAttributeS("nick", p_nickname));

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendStatusMessage(const QString &p_message)
{
    Xfire::Packet status(0x0020);
    status.addAttribute(new Xfire::StringAttributeB(0x2e, p_message));

    m_connection->write(status.toByteArray());
}

void XfireServer::sendFriendInvitation(QString &p_username, QString &p_message)
{
    Xfire::Packet foo(0x0006);
    foo.addAttribute(new Xfire::StringAttributeS("name", p_username));
    foo.addAttribute(new Xfire::StringAttributeS("msg", p_message));

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendFriendInvitationResponse(QString p_username, bool pResponse)
{
    Xfire::Packet *foo;

    foo = new Xfire::Packet(pResponse ? 0x0007 : 0x0008);
    foo->addAttribute(new Xfire::StringAttributeS("name", p_username));

    m_connection->write(foo->toByteArray());
}

void XfireServer::sendFriendNetworkRequest(QList<Xfire::SIDAttribute*> fofs)
{
    // FIXME: Not implemented yet
}

void XfireServer::sendIngameStatus(quint32 p_gameId, quint32 p_gameIp, quint32 p_gamePort)
{
    Xfire::Packet foo(0x0004);
    foo.addAttribute(new Xfire::Int32AttributeS("gameid", p_gameId));
    foo.addAttribute(new Xfire::Int32AttributeS("gip", p_gameIp));
    foo.addAttribute(new Xfire::Int32AttributeS("gport", p_gamePort));

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendP2pSession(const Xfire::SessionID &p_sid, quint32 p_ip, quint16 p_port, quint32 p_localIp, quint16 p_localPort, quint32 p_natType, const QString &p_salt)
{
    Xfire::Packet foo(0x0002);

    foo.addAttribute(new Xfire::SIDAttributeS("sid", p_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 2));
    peermsg->addAttribute(new Xfire::Int32AttributeS("ip", p_ip));
    peermsg->addAttribute(new Xfire::Int32AttributeS("port", p_port));
    peermsg->addAttribute(new Xfire::Int32AttributeS("localip", p_localIp));
    peermsg->addAttribute(new Xfire::Int32AttributeS("localport", p_localPort));
    peermsg->addAttribute(new Xfire::Int32AttributeS("status", p_natType));
    peermsg->addAttribute(new Xfire::StringAttributeS("salt", p_salt));
    foo.addAttribute(peermsg);

    m_connection->write(foo.toByteArray());
}

void XfireServer::sendTypingStatus(const Xfire::SessionID &p_sid, quint32 p_chatMessageIndex, bool p_isTyping)
{
    Xfire::Packet typing(0x0002);
    typing.addAttribute(new Xfire::SIDAttributeS("sid", p_sid));

    Xfire::ParentStringAttributeS *peermsg = new Xfire::ParentStringAttributeS("peermsg");
    peermsg->addAttribute(new Xfire::Int32AttributeS("msgtype", 3));
    peermsg->addAttribute(new Xfire::Int32AttributeS("imindex", p_chatMessageIndex));
    peermsg->addAttribute(new Xfire::Int32AttributeS("typing", p_isTyping ? 1 : 0));
    typing.addAttribute(peermsg);

    m_connection->write(typing.toByteArray());
}

void XfireServer::handlePacket(const Xfire::Packet *p_packet, XfireP2PSession *p_session)
{
    switch (p_packet->id())
    {
        // Login salt
    case 0x0080:
    {
        const Xfire::StringAttributeS *attr = static_cast<const Xfire::StringAttributeS*>(p_packet->getAttribute("salt"));

        if(!attr || attr->type() != Xfire::Attribute::String)
        {
            kDebug() << "Bad salt packet received";
            m_account->logOff(Kopete::Account::Unknown);
            return;
        }

        // Set salt and login
        m_salt = attr->string();
        login(m_salt);

        break;
    }

    // Wrong username or password
    case 0x0081:
    {
        // Set password as wrong and reconnect account
        m_account->password().setWrong(true);
        m_account->logOff(Kopete::Account::BadPassword);
        emit goOnline();

        break;
    }

    // Client information
    case 0x0082:
    {
        const Xfire::Int32AttributeS *uid = static_cast<const Xfire::Int32AttributeS*>(p_packet->getAttribute("userid"));
        const Xfire::SIDAttributeS *sid = static_cast<const Xfire::SIDAttributeS*>(p_packet->getAttribute("sid"));
        const Xfire::StringAttributeS *alias = static_cast<const Xfire::StringAttributeS*>(p_packet->getAttribute("nick"));

        if(!uid || !sid || uid->type() != Xfire::Attribute::Int32 || sid->type() != Xfire::Attribute::SID ||
            !alias || alias->type() != Xfire::Attribute::String)
        {
            kDebug() << "Bad client information packet received, ignoring";
            m_account->logOff(Kopete::Account::Unknown);
            return;
        }

        m_heartBeat->start(XF_HEARTBEAT_TIMEOUT); // Start the heartbeat
        emit ourStatusChanged(XfireProtocol::protocol()->XfireOnline); // Set online status in Kopete

        m_account->m_sid = sid->sid();

        break;
    }

    // Friends list
    case 0x0083:
    {
        const Xfire::ListAttributeS *friends = static_cast<const Xfire::ListAttributeS*>(p_packet->getAttribute("friends"));
        const Xfire::ListAttributeS *nick = static_cast<const Xfire::ListAttributeS*>(p_packet->getAttribute("nick"));
        const Xfire::ListAttributeS *uid = static_cast<const Xfire::ListAttributeS*>(p_packet->getAttribute("userid"));

        if(!friends || friends->type() != Xfire::Attribute::List || !nick || nick->type() != Xfire::Attribute::List ||
            !uid || uid->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < friends->elements().size(); i++)
        {
            // Add contact and update ID
            m_account->newContact(friends->elements().at(i).string->string(), nick->elements().at(i).string->string(), -1);
            m_account->updateContactID(friends->elements().at(i).string->string(), uid->elements().at(i).int32);
        }

        break;
    }

    // Friends list: online/offline
    case 0x0084:
    {
        const Xfire::ListAttributeS *uid = static_cast<const Xfire::ListAttributeS*>(p_packet->getAttribute("userid"));
        const Xfire::ListAttributeS *usid = static_cast<const Xfire::ListAttributeS*>(p_packet->getAttribute("sid"));

        if(!uid || uid->type() != Xfire::Attribute::List || !usid || usid->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < uid->elements().size(); i++)
        {
            Xfire::SIDAttribute *sid = usid->elements().at(i).sid;
            m_account->updateContactSID(uid->elements().at(i).int32, sid);
        }

        break;
    }

    // Received chat message
    case 0x0085:
    case 0x0002:
    {
        const Xfire::SIDAttributeS *sid = static_cast<const Xfire::SIDAttributeS*>(p_packet->getAttribute("sid"));
        const Xfire::ParentStringAttributeS *peermsg = static_cast<const Xfire::ParentStringAttributeS*>(p_packet->getAttribute("peermsg"));
        const Xfire::Int32AttributeS *msgtype = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("msgtype"));

        if(!sid || sid->type() != Xfire::Attribute::SID || !peermsg || peermsg->type() != Xfire::Attribute::ParentString ||
            !msgtype || msgtype->type() != Xfire::Attribute::Int32)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        XfireContact *from = static_cast<const XfireContact *>(m_account->findContact(sid->sid()));
        if(from != 0)
        {
            switch (msgtype->value())
            {
            // Instant message
            case 0:
            {
                const Xfire::Int32AttributeS *imindex = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("imindex"));
                const Xfire::StringAttributeS *im = static_cast<const Xfire::StringAttributeS*>(peermsg->getAttribute("im"));
                if(!imindex || imindex->type() != Xfire::Attribute::Int32 || !im || im->type() != Xfire::Attribute::String)
                {
                    kDebug() << "Invalid packet received, ignoring";
                    break;
                }

                if(p_packet->id() == 0x0085)
                {
                    from->receivedMessage(im->string());
                    sendChatConfirmation(sid->sid(), imindex->value());
                }
                else
                {
                    p_session->m_contact->receivedMessage(im->string());
                    p_session->sendMessageConfirmation(imindex->value());
                }

                break;
            }
            // ACK packet (confirmation)
            case 1:
            {
                const Xfire::Int32AttributeS *imindex = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("imindex"));
                if(!imindex || imindex->type() != Xfire::Attribute::Int32)
                {
                    kDebug() << "Invalid packet received, ignoring";
                    break;
                }

                if(p_packet->id() == 0x0085)
                    from->setMessageSucceeded();
                else
                    p_session->m_contact->setMessageSucceeded();

                break;
            }
            // P2P information
            case 2:
            {
                const Xfire::Int32AttributeS *ip = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("ip"));
                const Xfire::Int32AttributeS *port = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("port"));
                const Xfire::Int32AttributeS *localip = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("localip"));
                const Xfire::Int32AttributeS *localport = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("localport"));
                const Xfire::Int32AttributeS *status = static_cast<const Xfire::Int32AttributeS*>(peermsg->getAttribute("status"));
                const Xfire::StringAttributeS *salt = static_cast<const Xfire::StringAttributeS*>(peermsg->getAttribute("salt"));

                int natType = status->Int32;
                if(natType > 0)
                {
                    if(from->m_p2pCapable == XfireContact::XF_P2P_YES || from->m_p2pCapable == XfireContact::XF_P2P_UNKNOWN)
                    {
                        if(!m_account->isPeerToPeerEnabled())
                        {
                            sendP2pSession(from->m_sid, 0, 0, 0, 0, 0, salt->string());
                            kDebug() << from->m_username + ": peer to peer request denied";
                            break;
                        }

                        if(natType == 1 ||(( natType == 2 || natType == 3) && m_account->m_p2pConnection->m_natCheck->m_type == 1) ||
                                (natType == 4 &&(m_account->m_p2pConnection->m_natCheck->m_type == 1 || m_account->m_p2pConnection->m_natCheck->m_type == 4)))
                        {
                            from->m_p2pCapable = XfireContact::XF_P2P_YES;
                            kDebug() << from->m_username + ": compatible buddy";

                            if(from->m_p2pSession == NULL)
                                from->createP2pSession(salt->string());

                            from->m_p2pSession->m_natType = natType;

                            from->m_p2pSession->setLocalAddress(localip->value(), localport->value());
                            from->m_p2pSession->setRemoteAddress(ip->value(), port->value());
                        }
                        else
                        {
                            from->m_p2pCapable = XfireContact::XF_P2P_NO;
                            kDebug() << from->m_username + ": incompatible buddy";
                            delete from->m_p2pSession; // Remove p2p session
                        }

                        if(!from->m_p2pRequested)
                        {
                            sendP2pSession(from->m_sid, m_account->m_p2pConnection->m_natCheck->m_ips[0], m_account->m_p2pConnection->m_connection->localPort(),
                                           m_connection->localAddress().toIPv4Address(), m_account->m_p2pConnection->m_connection->localPort(),
                                           m_account->m_p2pConnection->m_natCheck->m_type, salt->string());

                            from->m_p2pRequested = TRUE;
                            kDebug() << from->m_username + ": peer to peer request received, sent own data";
                        }
                    }
                    else
                    {
                        sendP2pSession(from->m_sid, 0, 0, 0, 0, 0, salt->string());
                        kDebug() << from->m_username + ": peer to peer request denied";
                    }
                }
                else
                {
                    from->m_p2pCapable = XfireContact::XF_P2P_NO;
                    delete from->m_p2pSession;
                }

                break;
            }

            // Typing notification
            case 3:
            {
                if(p_packet->id() == 0x0085)
                    from->setTypingStatus(msgtype->value());
                else
                    p_session->m_contact->setTypingStatus(msgtype->value());

                break;
            }

            default:
                break;
            }
        }

        break;
    }

    // New version information
    case 0x0086:
    {
        const Xfire::ListAttributeS *version = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("version"));

        if(!version || version->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            break;
        }

        Xfire::Int32Attribute *v = version->elements().at(0).int32;
        kDebug() << "New version:" << v->value();

        m_account->configGroup()->writeEntry("ProtocolVersion", v->value());
        emit m_account->slotVersionUpdated();

        emit goOffline();
        emit goOnline();

        break;
    }

    // Game status of friends
    case 0x0087:
    {
        const Xfire::ListAttributeS *sid = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("sid"));
        const Xfire::ListAttributeS *gameid = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("gameid"));
        const Xfire::ListAttributeS *gip = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("gip"));
        const Xfire::ListAttributeS *gport = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("gport"));

        if(!sid || sid->type() != Xfire::Attribute::List || !gameid || gameid->type() != Xfire::Attribute::List ||!gip ||
            gip->type() != Xfire::Attribute::List || !gport || gport->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < sid->elements().size(); i++)
        {
            Xfire::SIDAttribute *sdid = sid->elements().at(i).sid;
            Xfire::Int32Attribute *id = gameid->elements().at(i).int32;
            Xfire::Int32Attribute *ip = gip->elements().at(i).int32;
            Xfire::Int32Attribute *port = gport->elements().at(i).int32;

            m_account->updateContactGameInformation(sdid->sid(), id->value() , ip->value(), port->value());
        }

        break;
    }

    // Friend network information
    case 0x0088:
    {
        kDebug() << "GOT FOF!";
        break;
    }

    // Pending friend invitations
    case 0x008a:
    {
        const Xfire::ListAttributeS *name = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("name"));
        const Xfire::ListAttributeS *nick = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("nick"));
        const Xfire::ListAttributeS *msg = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("msg"));

        if(!name || name->type() != Xfire::Attribute::List || !nick || nick->type() != Xfire::Attribute::List || !msg || msg->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < name->elements().size(); i++)
        {
            Xfire::StringAttribute *username = name->elements().at(i).string;
            kDebug() << "Friend invitation:" << username->string();

            Kopete::AddedInfoEvent *event = new Kopete::AddedInfoEvent(username->string(), m_account);
            QObject::connect(event, SIGNAL(actionActivated(uint)), this, SLOT(slotAddedInfoEventActionActivated(uint)));

            Kopete::AddedInfoEvent::ShowActionOptions actions = Kopete::AddedInfoEvent::AddAction;
            actions |= Kopete::AddedInfoEvent::BlockAction;

            event->showActions(actions);
            event->sendEvent();
        }

        break;
    }

    // Heartbeat
    case 0x0090:
    {
        m_connectionTimeout->start(XF_CONNECTION_TIMEOUT); // Start connection check timer or restart if needed
        break;
    }

    // Logged in somewhere else
    case 0x0091:
    {
        kDebug() << "Logged in somewhere else";
        m_account->logOff(Kopete::Account::OtherClient);

        break;
    }

    case 0x009a:
    {
        const Xfire::ListAttributeS *sid = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("sid"));
        const Xfire::ListAttributeS *msg = static_cast<const Xfire::ListAttributeS *>(p_packet->getAttribute("msg"));

        if(!sid || sid->type() != Xfire::Attribute::List || !msg || msg->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < sid->elements().size(); i++)
            m_account->setStatus(sid->elements().at(i).sid->sid(), msg->elements().at(i).string->string());

        break;
    }

    case 0x009e:
    {
        const Xfire::ListAttributeB *clanID = static_cast<const Xfire::ListAttributeB *>(p_packet->getAttribute(0x006c));
        const Xfire::ListAttributeB *clanLongName = static_cast<const Xfire::ListAttributeB *>(p_packet->getAttribute(0x0002));
        const Xfire::ListAttributeB *clanShortName = static_cast<const Xfire::ListAttributeB *>(p_packet->getAttribute(0x0072));
        const Xfire::ListAttributeB *clanType = static_cast<const Xfire::ListAttributeB *>(p_packet->getAttribute(0x0034));

        if(!clanID || clanID->type() != Xfire::Attribute::List || !clanLongName || clanLongName->type() != Xfire::Attribute::List ||
            !clanShortName || clanShortName->type() != Xfire::Attribute::List || !clanType || clanType->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        // Create groups (clans) if needed
        for(int i = 0; i < clanID->elements().size(); i++)
        {
            kDebug() << "Clan:" << clanLongName->elements().at(i).string->string();

            Kopete::Group *group = Kopete::ContactList::self()->findGroup(clanLongName->elements().at(i).string->string());
            group->setGroupId(clanID->elements().at(i).int32->value());
        }

        break;
    }

    case 0x009f:
    {
        const Xfire::Int32AttributeB *clanID = static_cast<const Xfire::Int32AttributeB*>(p_packet->getAttribute(0x006c));
        const Xfire::ListAttributeB *userID = static_cast<const Xfire::ListAttributeB*>(p_packet->getAttribute(0x0001));
        const Xfire::ListAttributeB *username = static_cast<const Xfire::ListAttributeB*>(p_packet->getAttribute(0x0002));
        const Xfire::ListAttributeB *nickname = static_cast<const Xfire::ListAttributeB*>(p_packet->getAttribute(0x000d));
        const Xfire::ListAttributeB *clanNickname = static_cast<const Xfire::ListAttributeB *>(p_packet->getAttribute(0x006d));
        // INFO: Unknown attribute 0x0074

        if(!clanID || clanID->type() != Xfire::Attribute::Int32 || !userID || userID->type() != Xfire::Attribute::List ||
            !username || username->type() != Xfire::Attribute::List || !nickname || nickname->type() != Xfire::Attribute::List ||
            !clanNickname || clanNickname->type() != Xfire::Attribute::List)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        for(int i = 0; i < userID->elements().size(); i++)
        {
            kDebug() << "Clan friend: " + username->elements().at(i).string->string();

            // Add the contact and update the ID
            uint cid = Kopete::ContactList::self()->group(clanID->value())->groupId();
            m_account->newContact(username->elements().at(i).string->string(), nickname->elements().at(i).string->string(), cid);
            m_account->updateContactID(username->elements().at(i).string->string(), userID->elements().at(i).int32);
        }

        break;
    }

    case 0x00ae:
    {
        const Xfire::Int32AttributeB *sid = static_cast<const Xfire::Int32AttributeB *>(p_packet->getAttribute(0x0001));
        const Xfire::Int32AttributeB *avatarNumber = static_cast<const Xfire::Int32AttributeB *>(p_packet->getAttribute(0x001f));

        if(!sid || sid->type() != Xfire::Attribute::Int32 || !avatarNumber || avatarNumber->type() != Xfire::Attribute::Int32)
        {
            kDebug() << "Invalid packet received, ignoring";
            return;
        }

        // Update contact avatar
        XfireContact *c = static_cast<XfireContact*>(m_account->findContact(sid->value()));
        if(c != 0)
            c->updateAvatar(avatarNumber->value());

        break;
    }

    default:
        break;
    }
}

void XfireServer::slotAddedInfoEventActionActivated(uint p_actionId)
{
    Kopete::AddedInfoEvent *event = dynamic_cast<Kopete::AddedInfoEvent *>(sender());
    if(!event)
        return;

    switch (p_actionId)
    {
    case Kopete::AddedInfoEvent::AddContactAction:
        sendFriendInvitationResponse(event->contactId(), true);
        break;
    case Kopete::AddedInfoEvent::BlockAction:
        sendFriendInvitationResponse(event->contactId(), false);
        break;
    }
}

void XfireServer::slotConnectionInterrupted(QAbstractSocket::SocketError p_error)
{
    kDebug() << "Connection interrupted";
    m_account->logOff(Kopete::Account::ConnectionReset);
}

void XfireServer::slotSendHeartBeat()
{
    Xfire::Packet heartbeat(0x000D);

    heartbeat.addAttribute(new Xfire::Int32AttributeS("value", 0));
    heartbeat.addAttribute(new Xfire::ListAttributeS("stats", Xfire::ListAttribute::Int32));

    m_connection->write(heartbeat.toByteArray());
}
