/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#ifndef NETSIEGE_CLIENTSESSION_H
#define NETSIEGE_CLIENTSESSION_H

#include "udpconnection.h"
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace network {
// namespace pb {
//     class C2SMessage;
// }

// typedef std::shared_ptr<pb::C2SMessage> C2SMessage_ptr;

class NetworkServer;

class ClientSession : public std::enable_shared_from_this<ClientSession>,
                      public UdpConnection<int, pb::C2SMessage, pb::S2CMessage, NetworkServer>
{
public:
    ClientSession(const size_t& client_id, const udp::endpoint myEndpoint, NetworkServer* server);
    ~ClientSession();

    void inputPackage(const package_buffer_t& package);

    bool isActive() const { return m_isActive; }
    void isActive(bool active) { m_isActive = active; }

    size_t clientId() { return m_clientId; }

private:
    size_t m_clientId;
    bool m_isActive = false;
};

}

#endif // NETSIEGE_CLIENTSESSION_H
