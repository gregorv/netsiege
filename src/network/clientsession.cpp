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

#include "clientsession.h"
#include "network/network.pb.h"
#include "networkserver.h"

namespace network {

ClientSession::ClientSession(const size_t& client_id, const udp::endpoint myEndpoint, network::NetworkServer* server)
: UdpConnection(myEndpoint, server, server), m_clientId(client_id)
{
}

ClientSession::~ClientSession()
{

}

void ClientSession::inputPackage(const package_buffer_t& package)
{
    pb::C2SMessage msg;
    msg.ParseFromArray(&package.front(), package.size());
    if(!parsePackage(msg)) {
        // TODO Client negotiation
    }
}

}
